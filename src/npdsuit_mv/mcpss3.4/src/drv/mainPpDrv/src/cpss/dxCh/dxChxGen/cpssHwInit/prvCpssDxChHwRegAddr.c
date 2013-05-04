/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChHwRegAddr.c
*
* DESCRIPTION:
*       This file includes definitions of all different Cheetah registers
*       addresses to be used by all functions accessing PP registers.
*       a variable of type PRV_CPSS_DXCH_PP_REGS_ADDR_STC should be allocated
*       for each PP type.
*
* FILE REVISION NUMBER:
*       $Revision: 1.4 $
*
*******************************************************************************/
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChModuleConfig.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>
#include <cpss/generic/cpssHwInit/private/prvCpssHwInit.h>
#include <cpss/generic/port/private/prvCpssPortTypes.h>

/* set register address for register that is bmp of ports BUT for only 28 ports ,
   CPU in bit 31

   macro duplicate address to the 2 register addresses , so CPU port can be use
   as 'bit 63' --> like in device that TRULY support bmp of 64 ports
*/
#define PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(regArr,startAddr) \
    regArr[0] = regArr[1] = (startAddr)


/* set registers addresses for register that are 'ports bmp' with consecutive addresses */
#define PORTS_BMP_REG_SET_MAC(regArr,startAddr) \
    regArr[0] = (startAddr);                    \
    regArr[1] = (startAddr) + 4;

/* set registers addresses according to formula */
#define FORMULA_SINGLE_REG_MAC(regArr,startAddr,step,numSteps) \
    {                                                          \
        GT_U32  _ii;                                           \
        for(_ii = 0 ; _ii < (numSteps) ; _ii++)                \
        {                                                      \
            regArr[_ii] = (startAddr) + (_ii * (step));        \
        }                                                      \
    }

/* set registers addresses according to formula */
#define FORMULA_DOUBLE_REG_MAC(regArr,startAddr,step1,numSteps1,step2,numSteps2) \
    {                                                          \
        GT_U32  _ii,_jj;                                       \
        for(_ii = 0 ; _ii < (numSteps1) ; _ii++)               \
        {                                                      \
            for(_jj = 0 ; _jj < (numSteps2) ; _jj++)           \
            {                                                  \
                regArr[_ii][_jj] = (startAddr) + (_ii * (step1)) + (_jj * (step2)); \
            }                                                  \
        }                                                      \
    }


/* set registers addresses according to formula , with start index in array of addresses */
#define FORMULA_SINGLE_REG_WITH_INDEX_MAC(regArr,startAddr,step,numSteps,startIndex) \
    {                                                          \
        GT_U32  _ii;                                           \
        for(_ii = 0 ; _ii < (numSteps) ; _ii++)                \
        {                                                      \
            regArr[(startIndex)+_ii] = (startAddr) + (_ii * (step)); \
        }                                                      \
    }

/* macro for Lion (4 port groups) for 16 local port registers , duplicated for
   all port groups */
#define LION_16_LOCAL_PORTS_TO_64_GLOBAL_PORTS_REG_SET_MAC(regArr,startAddr,step) \
    FORMULA_SINGLE_REG_WITH_INDEX_MAC(regArr,startAddr,step,16, 0);           \
    FORMULA_SINGLE_REG_WITH_INDEX_MAC(regArr,startAddr,step,16,16);           \
    FORMULA_SINGLE_REG_WITH_INDEX_MAC(regArr,startAddr,step,16,32);           \
    FORMULA_SINGLE_REG_WITH_INDEX_MAC(regArr,startAddr,step,16,48)




/*******************************************************************************
* defaultAddressUnusedSet
*
* DESCRIPTION:
*       This function set all the register addresses as 'unused'.
*
* INPUTS:
*       startPtr    - pointer to start of register addresses
*       numOfRegs   - number of registers to initialize
* OUTPUTS:
*       startPtr    - pointer to start of register addresses that where initialized
*
* RETURNS:
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void defaultAddressUnusedSet
(
    INOUT void   *startPtr,
    IN GT_U32   numOfRegs
)
{
    GT_U32        ii;
    GT_U32       *regsAddrPtr32;
    GT_U32        regsAddrPtr32Size;

    /*Fill the all words in the pointer with initial value*/
    regsAddrPtr32       = startPtr;
    regsAddrPtr32Size   = numOfRegs;

    for( ii = 0; ii < regsAddrPtr32Size; ii++ )
    {
        regsAddrPtr32[ii] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    }

    return;
}

/*******************************************************************************
* allocAddressesMemory
*
* DESCRIPTION:
*       allocate memory for registers addresses and initialize as 'unused'.
*
* INPUTS:
*       numOfBytes   - number of bytes to initialize
* OUTPUTS:
*       none
*
* RETURNS:
*       pointer to start of allocated and initialized memory
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void* allocAddressesMemory
(
    IN GT_U32   numOfBytes
)
{
    void* memoryAlloc = cpssOsMalloc(numOfBytes);

    if(memoryAlloc)
    {
        /* initialize the memory */
        defaultAddressUnusedSet(memoryAlloc,numOfBytes / 4);
    }

    return memoryAlloc;
}


/*******************************************************************************
* cheetahBridgeRegsAlloc
*
* DESCRIPTION:
*       This function allocates memory for bridge registers struct for devices
*       belong to REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with bridge regs init
*
* RETURNS:
*       GT_OK               - on success, or
*       GT_OUT_OF_CPU_MEM   - om malloc failed
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS cheetahBridgeRegsAlloc
(
    IN  GT_U32                              devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32  size;

    size = sizeof(GT_U32) * PRV_CPSS_PP_MAC(devNum)->numOfPorts;
    /* allocate all arrays for per port registers */
    addrPtr->bridgeRegs.portControl = allocAddressesMemory(size);
    if (addrPtr->bridgeRegs.portControl == NULL)
        return GT_OUT_OF_CPU_MEM;

    addrPtr->bridgeRegs.swPortControlBookmark  = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
    addrPtr->bridgeRegs.swPortControlType      = PRV_CPSS_SW_TYPE_PTR_FIELD_CNS;
    addrPtr->bridgeRegs.swPortControlSize      = size;

    addrPtr->bridgeRegs.vlanRegs.portPrvVlan = allocAddressesMemory(size);
    if (addrPtr->bridgeRegs.vlanRegs.portPrvVlan == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    addrPtr->bridgeRegs.vlanRegs.swPortPrvVlanBookmark   =
        PRV_CPSS_SW_PTR_BOOKMARK_CNS;
    addrPtr->bridgeRegs.vlanRegs.swPortPrvVlanType       =
        PRV_CPSS_SW_TYPE_PTR_FIELD_CNS | PRV_CPSS_SW_TYPE_WRONLY_CNS;
    addrPtr->bridgeRegs.vlanRegs.swPortPrvVlanSize       = size;

    /* the cheetah has 16 registers of "multicast range filters"
       save the first value of the register only !

       offset between the register is : 0x1000
    */
    addrPtr->bridgeRegs.macRangeFltrBase = allocAddressesMemory(4 * sizeof(GT_U32));
    if (addrPtr->bridgeRegs.macRangeFltrBase == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    addrPtr->bridgeRegs.swMacRangeFltrBaseBookmark = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
    addrPtr->bridgeRegs.swMacRangeFltrBaseType    = PRV_CPSS_SW_TYPE_PTR_FIELD_CNS;
    addrPtr->bridgeRegs.swMacRangeFltrBaseSize    = 4 * sizeof(GT_U32);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* need to use table PRV_CPSS_DXCH_LION_TABLE_TXQ_EGRESS_STC_E
           instead of registers */
        addrPtr->sFlowRegs.egrStcTblWord0 = allocAddressesMemory(size);
        if (addrPtr->sFlowRegs.egrStcTblWord0 == NULL)
        {
            return GT_OUT_OF_CPU_MEM;
        }

        addrPtr->sFlowRegs.swEgrStcTblW0Bookmark = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
        addrPtr->sFlowRegs.swEgrStcTblW0Type     = PRV_CPSS_SW_TYPE_PTR_FIELD_CNS;
        addrPtr->sFlowRegs.swEgrStcTblW0Size     = size;

        addrPtr->sFlowRegs.egrStcTblWord1 = allocAddressesMemory(size);
        if (addrPtr->sFlowRegs.egrStcTblWord1 == NULL)
        {
            return GT_OUT_OF_CPU_MEM;
        }

        addrPtr->sFlowRegs.swEgrStcTblW1Bookmark = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
        addrPtr->sFlowRegs.swEgrStcTblW1Type     = PRV_CPSS_SW_TYPE_PTR_FIELD_CNS;
        addrPtr->sFlowRegs.swEgrStcTblW1Size     = size;

        addrPtr->sFlowRegs.egrStcTblWord2 = allocAddressesMemory(size);
        if (addrPtr->sFlowRegs.egrStcTblWord2 == NULL)
        {
            return GT_OUT_OF_CPU_MEM;
        }

        addrPtr->sFlowRegs.swEgrStcTblW2Bookmark = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
        addrPtr->sFlowRegs.swEgrStcTblW2Type     = PRV_CPSS_SW_TYPE_PTR_FIELD_CNS;
        addrPtr->sFlowRegs.swEgrStcTblW2Size     = size;
    }

    return GT_OK;
}

/*******************************************************************************
* cheetahBridgeRegsInit
*
* DESCRIPTION:
*       This function initializes the registers bridge struct for devices that
*       belong to REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with bridge regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetahBridgeRegsInit
(
    IN GT_U32          devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U8   i;
    GT_U32  offset;
    /* init all arrays of per port registers */
    for (i = 0; i < PRV_CPSS_PP_MAC(devNum)->numOfPorts; i++)
    {
        offset = i * 0x1000;

        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            offset = (i & 0xf) * 0x1000;

            if((i & 0xf) > 0xb)
            {
                addrPtr->bridgeRegs.portControl[i]          =
                addrPtr->bridgeRegs.vlanRegs.portPrvVlan[i] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
                continue;
            }
        }

        /* bridge configuration registers 0..1 */
        addrPtr->bridgeRegs.portControl[i] = 0x02000000 + offset;

        addrPtr->bridgeRegs.vlanRegs.portPrvVlan[i]    = 0x02000010 + offset;


        /* both "port based" and "port and protocol based" */
        /* VLAN and QoS configuration tables */
        /* have only indirect and "control + data" access */
        /* not used portVid and protoVidBased*/

        /* brdByteLimit - is in bridge configuration register1 */
        /* use portPrvVlan instead of brdByteLimit */

        /* brdByteWindow - in global register 0x02040140 */
    }

    addrPtr->bridgeRegs.cpuPortControl = 0x0203F000;

    /* QoS Profile registers */
    addrPtr->haRegs.qosProfile2ExpTable = 0x07800300;
    addrPtr->bridgeRegs.qosProfileRegs.dscp2QosProfileMapTable = 0x0B800450;
    addrPtr->bridgeRegs.qosProfileRegs.up2QosProfileMapTable = 0x0B8004A0;
    addrPtr->bridgeRegs.qosProfileRegs.dscp2DscpMapTable = 0x0B800400;
    addrPtr->bridgeRegs.qosProfileRegs.exp2QosProfileMapTable = 0x0B8004A8;
    addrPtr->bridgeRegs.qosProfileRegs.macQosTable = 0x02000200;

    for (i = 0; i < 2; i++)
    {
        offset = i * 20;
        addrPtr->bridgeRegs.brgCntrSet[i].cntrSetCfg   = 0x020400DC + offset;
        addrPtr->bridgeRegs.brgCntrSet[i].inPckt       = 0x020400E0 + offset;
        addrPtr->bridgeRegs.brgCntrSet[i].inFltPckt    = 0x020400E4 + offset;
        addrPtr->bridgeRegs.brgCntrSet[i].secFltPckt   = 0x020400E8 + offset;
        addrPtr->bridgeRegs.brgCntrSet[i].brgFltPckt   = 0x020400EC + offset;
    }

    addrPtr->bridgeRegs.brgMacCntr0 = 0x020400B0;
    /* UP mapped not to TC (and to DP) only, but to QOS profile */
    /* see regs 0x0B8004A0, 0x0B8004A4 */
    /* map TC to ... not relevant to cheetah */
    /* the CpuCode table entry contais TC and DP */
    /* regs 0x0B000030 and 0x0B003000 */

    addrPtr->bridgeRegs.deviceTable = 0x06000068;

    /* Bridge Ingress Drop Counter Reg */
    addrPtr->bridgeRegs.dropIngrCntr = 0x02040150;

    addrPtr->bridgeRegs.hostInPckt = 0x020400BC;
    addrPtr->bridgeRegs.hostOutPckt = 0x020400C0;
    addrPtr->bridgeRegs.hostOutMcPckt = 0x020400CC;
    addrPtr->bridgeRegs.hostOutBrdPckt = 0x020400D0;

    addrPtr->bridgeRegs.ingFltStatus = 0x020401A8;/*Security Breach Status Register2*/
    addrPtr->bridgeRegs.egressFilter = 0x01800010;
    addrPtr->bridgeRegs.egressFilter2 = 0x01800014;
    addrPtr->bridgeRegs.mcastLocalEnableConfig = 0x01800008;
    addrPtr->bridgeRegs.egressFilterIpMcRouted = 0x018001D0;

    addrPtr->bridgeRegs.l2IngressControl = 0x0B800000;
    addrPtr->bridgeRegs.learnedDisc = 0x06000030;

    /*
        Classified Distribution Table 379:
        IEEE Reserved Multicast Configuration Register<n> (0<=n<16)
        Offset:  Reg0: 0x02000800, Reg1:0x02001800... Reg15:0x0200F800
        (16 Registers in step of 0x1000)
    */
    addrPtr->bridgeRegs.macRangeFltrBase[0] = 0x02000800;

    /*
     *  There are two IPv6 ICMP Message Type Configuration registers
     *  with the offset 0x1000. Only the base is allocated
    */
    addrPtr->bridgeRegs.ipv6IcmpMsgTypeBase  = 0x02010800;
    /* IPv6 ICMP Message Type Cmd */
    addrPtr->bridgeRegs.ipv6IcmpMsgCmd      = 0x02012800;

    /*
     *  There are 8 IPv6 Link Local Configuration registers
     *  with the offset 0x1000. Only the base is allocated
    */
    addrPtr->bridgeRegs.ipv6McastLinkLocalBase  = 0x02028800;

    /*
     *  There are 8 IPv4 Link Local Configuration registers
     *  with the offset 0x1000. Only the base is allocated
    */
    addrPtr->bridgeRegs.ipv4McastLinkLocalBase  = 0x02020800;

    addrPtr->bridgeRegs.ipv6McastSolicitNodeAddrBase = 0x0B800050;

    addrPtr->bridgeRegs.ipv6McastSolicitNodeMaskBase = 0x0B800060;

    /* Number of Trunk Members Table Entry<n> (0<=n<16)  */
    /* set only the base address */
     /* 8 trunks in each address  starting trunkId = 1 */
    addrPtr->bridgeRegs.trunkTable = 0x02000804;

    addrPtr->bridgeRegs.macTblAction0 = 0x06000004;
    addrPtr->bridgeRegs.macTblAction1 = 0x06000008;
    addrPtr->bridgeRegs.macTblAction2 = 0x06000020;
    addrPtr->bridgeRegs.matrixPckt = 0x020400D4;

    /* two VLAN ranges */
    addrPtr->bridgeRegs.vlan0 = 0x020400A8;
    /* two protocol IDs 0x8100 */
    addrPtr->bridgeRegs.vlan1 = 0x0B800004;

    addrPtr->bridgeRegs.vlanRegs.protoEncBased = 0x0B800520;
    addrPtr->bridgeRegs.vlanRegs.protoEncBased1 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;

    /* 4 registers 0x0B800500-0x0B80050C for 8 protocols */
    addrPtr->bridgeRegs.vlanRegs.protoTypeBased = 0x0B800500;

    /* Egress Vlan Ether Type Configuration Register */
    addrPtr->bridgeRegs.egressVlanEtherTypeConfig = 0x07800018;

    /* Egress Vlan Ether Type Select Register */
    addrPtr->bridgeRegs.egressVlanEtherTypeSel = 0x0780001C;

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* override */
        addrPtr->bridgeRegs.egressVlanEtherTypeConfig = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->bridgeRegs.egressVlanEtherTypeSel    = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    }

    if (PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
    {
        /* MAC error indication port configuration register */
        addrPtr->bridgeRegs.macErrorIndicationPortConfReg = PRV_CPSS_SW_PTR_ENTRY_UNUSED;

        /* MAC error indication status register */
        addrPtr->bridgeRegs.macErrorIndicationStatusReg[0] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->bridgeRegs.macErrorIndicationStatusReg[1] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    }
    else
    {
        /* MAC error indication port configuration register */
        addrPtr->bridgeRegs.macErrorIndicationPortConfReg = 0x0780004C;

        /* MAC error indication status register */
        addrPtr->bridgeRegs.macErrorIndicationStatusReg[0] = 0x07800050;
        addrPtr->bridgeRegs.macErrorIndicationStatusReg[1] = 0x07800054;
    }

    addrPtr->bridgeRegs.bridgeGlobalConfigRegArray[0] = 0x02040000;
    addrPtr->bridgeRegs.bridgeGlobalConfigRegArray[1] = 0x02040004;
    addrPtr->bridgeRegs.bridgeGlobalConfigRegArray[2] = 0x0204000C;

    addrPtr->bridgeRegs.bridgeGlobalSecurBreachDropCounter   = 0x02040104;
    addrPtr->bridgeRegs.bridgePortVlanSecurBreachDropCounter = 0x02040108;

    addrPtr->bridgeRegs.bridgeRateLimitConfigRegArray[0] = 0x02040140;
    addrPtr->bridgeRegs.bridgeRateLimitConfigRegArray[1] = 0x02040144;

    addrPtr->bridgeRegs.bridgeSecurBreachStatusRegArray[0] = 0x020401A0;
    addrPtr->bridgeRegs.bridgeSecurBreachStatusRegArray[1] = 0x020401A4;
    addrPtr->bridgeRegs.bridgeSecurBreachStatusRegArray[2] = 0x020401A8;

    addrPtr->bridgeRegs.macControlReg  = 0x06000000;

    /* IPv6 MC Bridging Bytes Selection Configuration Register */
    addrPtr->bridgeRegs.bridgeIpv6BytesSelection = 0x02040010;

    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->haRegs.haVlanTransEnReg,0x07800130);

    for (i = 0; i < 4; i++)
    {
        addrPtr->bridgeRegs.bridgeIngressVlanEtherTypeTable[i] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->haRegs.bridgeEgressVlanEtherTypeTable[i] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->haRegs.bridgeEgressPortTag0TpidSelect[i] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->haRegs.bridgeEgressPortTag1TpidSelect[i] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    }

    for (i = 4; i < (PORTS_NUM_CNS/8); i++)
    {
        addrPtr->haRegs.bridgeEgressPortTag0TpidSelect[i] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->haRegs.bridgeEgressPortTag1TpidSelect[i] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    }

    for (i = 0; i < 32; i++)
    {
        addrPtr->bridgeRegs.bridgeIngressVlanSelect[i] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    }

    addrPtr->bridgeRegs.ieeeReservedMcastProfileSelect0 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->bridgeRegs.ieeeReservedMcastProfileSelect1 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;

    addrPtr->bridgeRegs.macRangeFltrBase[1] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->bridgeRegs.macRangeFltrBase[2] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->bridgeRegs.macRangeFltrBase[3] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
}

/*******************************************************************************
* cheetahGlobalRegsInit
*
* DESCRIPTION:
*       This function initializes the global registers struct for devices that
*       belong to REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with global regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetahGlobalRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U8   i;

    for (i = 0; i < 4; i++)
    {
        addrPtr->globalRegs.addrUpdate[i] = 0x06000040 + i * 4;
    }
    addrPtr->globalRegs.addrUpdateControlReg = 0x06000050;

    addrPtr->globalRegs.globalControl          = 0x00000058;
    addrPtr->globalRegs.extendedGlobalControl  = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->globalRegs.extendedGlobalControl2 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->globalRegs.sampledAtResetReg      = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->globalRegs.addrCompletion         = 0x00000000;
    addrPtr->globalRegs.auQBaseAddr            = 0x000000C0;
    addrPtr->globalRegs.auQControl             = 0x000000C4;
    addrPtr->globalRegs.fuQBaseAddr            = 0x000000C8;
    addrPtr->globalRegs.fuQControl             = 0x000000CC;
    addrPtr->globalRegs.interruptCoalescing    = 0x000000E0;
    addrPtr->globalRegs.dummyReadAfterWriteReg = 0x000000F0;

    addrPtr->globalRegs.cpuPortCtrlReg         = 0x000000A0;
}


/*******************************************************************************
* cheetahBufMngRegsAlloc
*
* DESCRIPTION:
*       This function allocates  memory for the buffer management registers
*       struct for devices that belong to REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with buffer management regs init
*
* RETURNS:
*       GT_OK               - on success, or
*       GT_OUT_OF_CPU_MEM   - om malloc failed
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS cheetahBufMngRegsAlloc
(
    IN  GT_U32                              devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32  size;

    /* allocate array for per profile registers */
    size = sizeof(struct _bufMngPerPort) * PRV_CPSS_PP_MAC(devNum)->numOfPorts;
    addrPtr->bufferMng.bufMngPerPort = allocAddressesMemory(size);
    if (addrPtr->bufferMng.bufMngPerPort == NULL)
        return GT_OUT_OF_CPU_MEM;

    return GT_OK;
}

/*******************************************************************************
* cheetahBufMngRegsInit
*
* DESCRIPTION:
*       This function initializes the buffer management registers struct
*       for devices that belong to REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with buffer management regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetahBufMngRegsInit
(
    IN GT_U32          devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U8   i;
    GT_U32  offset;
    /* allocate array for per port registers */
    for(i = 0; i < 32; i++)
    {
        addrPtr->bufferMng.devMapTbl[i] = 0x01A40004 + i*0x10;
        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            /* between every target device we have 0x80 (the first 0x10 already added) */
            addrPtr->bufferMng.devMapTbl[i] += i*0x70;
        }
    }

    for(i = 0; i < 32; i++)
    {
        addrPtr->bufferMng.srcIdEggFltrTbl[i] = 0x01A4000C + i*0x10;
    }


    /* Trunk<n> Non-Trunk Members Table (0<=n<128) */
    /* set base address for low bits , base address for low bits
        ---> in index 0 */
    /* use CHEETAH_TRUNK_NON_TRUNK_TABLE_OFFSET_CNS for offset */
    addrPtr->bufferMng.trunkTblCfgRegs.baseAddrNonTrunk = 0x01A40000;

    /* use CHEETAH_TRUNK_DESIGNATED_PORTS_TABLE_OFFSET_CNS for offset */
    addrPtr->bufferMng.trunkTblCfgRegs.baseAddrTrunkDesig = 0x01A40008;

    /* Port<4n,4n+1,4n+2,4n+3> TrunkNum Configuration Register<n> (0<=n<7) */
    /* num of registers : (corePpDevs[devNum]->numOfPorts / 4) + 1 */
    /* use CHEETAH_TRUNK_ID_CONIFG_TABLE_OFFSET_CNS for offset */
    for (i = 0; i < 7; i++)
    {
        addrPtr->haRegs.trunkNumConfigReg[i] = 0x0780002C + 4*i;
    }

    /* init all arrays of per profiles registers */
    addrPtr->bufferMng.swBufMngPerPortBookmark = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
    addrPtr->bufferMng.swBufMngPerPortType     = PRV_CPSS_SW_TYPE_PTR_FIELD_CNS;
    addrPtr->bufferMng.swBufMngPerPortSize     =
        sizeof(struct _bufMngPerPort) * PRV_CPSS_PP_MAC(devNum)->numOfPorts;
    addrPtr->bufferMng.bufMngAgingConfig = 0x0300000C;
    addrPtr->bufferMng.bufMngGlobalLimit = 0x03000000;
    addrPtr->bufferMng.cpuRxBufLimit = 0x03000020;
    addrPtr->bufferMng.cpuRxBufCount = 0x0300017C;

    addrPtr->bufferMng.eqBlkCfgRegs.trapSniffed = 0x0B000000;
    addrPtr->bufferMng.eqBlkCfgRegs.sniffQosCfg = 0x0B000004;
    addrPtr->bufferMng.eqBlkCfgRegs.sniffPortsCfg = 0x0B000008;

    addrPtr->bufferMng.eqBlkCfgRegs.cpuTargDevConfReg0 = 0x0B000010;
    addrPtr->bufferMng.eqBlkCfgRegs.cpuTargDevConfReg1 = 0x0B000014;

    addrPtr->bufferMng.eqBlkCfgRegs.trSrcSniff = 0x0B00000C;

    addrPtr->bufferMng.eqBlkCfgRegs.ingressDropCntrReg = 0x0B000040;


    addrPtr->bufferMng.bufLimitsProfile.associationReg0 = 0x03000030;
    addrPtr->bufferMng.bufLimitsProfile.associationReg1 = 0x03000034;

    addrPtr->bufferMng.bufMngGigaPortGroupLimit  = 0x03000004;
    addrPtr->bufferMng.bufMngHyperGStackPortGroupLimit  = 0x03000008;

    addrPtr->bufferMng.bufMngMetalFix = 0x03000060;

    for (i = 0; i < PRV_CPSS_DXCH_PROFILES_NUM_CNS; i++)
    {
        addrPtr->bufferMng.bufLimitProfileConfig[i] = 0x03000020 + i*4;
    }

    for (i = 0; i < PRV_CPSS_PP_MAC(devNum)->numOfPorts; i++)
    {
        offset = i * 4;
        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            offset = (i & 0xf) * 4;
            if((i & 0xf) > 0xb)
            {
                addrPtr->bufferMng.bufMngPerPort[i].portBufAllocCounter =
                    PRV_CPSS_SW_PTR_ENTRY_UNUSED;
                continue;
            }
        }
        addrPtr->bufferMng.bufMngPerPort[i].portBufAllocCounter = 0x03000100 + offset;
    }
    addrPtr->bufferMng.cpuBufMngPerPort.portBufAllocCounter = 0x0300017C;
    addrPtr->bufferMng.totalBufAllocCntr = 0x03000058;
    addrPtr->bufferMng.bufMngPacketCntr  = 0x0300005C;
    addrPtr->bufferMng.netPortGroupAllocCntr = 0x03000080;
    addrPtr->bufferMng.stackPortGroupAllocCntr = 0x03000084;

}

/*******************************************************************************
* cheetahEgrTxRegsAlloc
*
* DESCRIPTION:
*       This function allocates memory for the Egress and Transmit (Tx) Queue
*       Configuration registers struct for devices that belong to
*       REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       GT_OK               - on success, or
*       GT_OUT_OF_CPU_MEM   - om malloc failed
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS cheetahEgrTxRegsAlloc
(
    IN GT_U32          devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32  size;

    if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return GT_OK;
    }

    size = sizeof(dxch_txPortRegs) * PRV_CPSS_PP_MAC(devNum)->numOfPorts;
    addrPtr->egrTxQConf.txPortRegs = allocAddressesMemory(size);
    if (addrPtr->egrTxQConf.txPortRegs == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    return GT_OK;
}

/*******************************************************************************
* cheetahEgrTxRegsInit
*
* DESCRIPTION:
*       This function initializes the Egress and Transmit (Tx) Queue
*       Configuration registers struct for devices that belong to
*       REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetahEgrTxRegsInit
(
    IN GT_U32          devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U8   i,j;
    GT_U32  size;
    GT_U32  offset;

    if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return;
    }

    size = sizeof(dxch_txPortRegs) * PRV_CPSS_PP_MAC(devNum)->numOfPorts;

    addrPtr->egrTxQConf.swTxPortRegsBookmark   = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
    addrPtr->egrTxQConf.swTxPortRegsType   = PRV_CPSS_SW_TYPE_PTR_FIELD_CNS;
    addrPtr->egrTxQConf.swTxPortRegsSize       = size;

    /* init all arrays of per profiles registers */
    for (i = 0; i < PRV_CPSS_DXCH_PROFILES_NUM_CNS; i++)
    {
        /* WRR output algorithm configured */
        /* via 4 Transmit scheduler  profiles */
        offset = i * 0x10;
        addrPtr->egrTxQConf.txPortRegs[i].wrrWeights0      = 0x01800040 + offset;
        addrPtr->egrTxQConf.txPortRegs[i].wrrWeights1      = 0x01800044 + offset;
        addrPtr->egrTxQConf.txPortRegs[i].wrrStrictPrio    = 0x01800048 + offset;

    }

    /* init not used regs */
    for (/* i continue from prev loop */;
         i < PRV_CPSS_PP_MAC(devNum)->numOfPorts; i++)
    {
        /* WRR output algorithm configured */
        /* via 4 Transmit scheduler  profiles */
        addrPtr->egrTxQConf.txPortRegs[i].wrrWeights0 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->egrTxQConf.txPortRegs[i].wrrWeights1 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->egrTxQConf.txPortRegs[i].wrrStrictPrio = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    }

    addrPtr->egrTxQConf.l2PortIsolationTableBase = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->egrTxQConf.l3PortIsolationTableBase = PRV_CPSS_SW_PTR_ENTRY_UNUSED;


    /* init all arrays of per port registers */
    for (i = 0; i < PRV_CPSS_PP_MAC(devNum)->numOfPorts; i++)
    {
        /* per traffic class */
        for (j = 0; j < 8; j++)
        {
            offset = j * 0x8000 + i * 0x200;
            if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
            {
                offset = j * 0x8000 + (i & 0xf) * 0x200;

                if((i & 0xf) > 0xb)
                {
                    addrPtr->egrTxQConf.txPortRegs[i].prioTokenBuck[j]   =
                    addrPtr->egrTxQConf.txPortRegs[i].prioTokenBuckLen[j]=
                    addrPtr->egrTxQConf.txPortRegs[i].tcDescrCounter[j]  =
                    addrPtr->egrTxQConf.txPortRegs[i].tcBufferCounter[j] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
                    continue;
                }
            }
            addrPtr->egrTxQConf.txPortRegs[i].prioTokenBuck[j]     = 0x01AC0000 + offset;
            /* such current state info unreachable */
            addrPtr->egrTxQConf.txPortRegs[i].prioTokenBuckLen[j]  = (0x01AC0004 + offset);
            addrPtr->egrTxQConf.txPortRegs[i].tcDescrCounter[j]    = 0x01880000 + offset;
            addrPtr->egrTxQConf.txPortRegs[i].tcBufferCounter[j]   = 0x01900000 + offset;
        }

        offset = i * 0x200;
        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            offset = (i & 0xf) * 0x200;

            if((i & 0xf) > 0xb)
            {
                addrPtr->egrTxQConf.txPortRegs[i].tokenBuck      =
                addrPtr->egrTxQConf.txPortRegs[i].tokenBuckLen   =
                addrPtr->egrTxQConf.txPortRegs[i].txConfig       =
                addrPtr->egrTxQConf.txPortRegs[i].wdTxFlush      =
                addrPtr->egrTxQConf.txPortRegs[i].descrCounter   =
                addrPtr->egrTxQConf.txPortRegs[i].bufferCounter  = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
                continue;
            }
        }
        addrPtr->egrTxQConf.txPortRegs[i].tokenBuck        = 0x01AC0008 + offset;
        addrPtr->egrTxQConf.txPortRegs[i].tokenBuckLen     = (0x01AC000C + offset);
        addrPtr->egrTxQConf.txPortRegs[i].txConfig         = 0x01800080 + offset;
        addrPtr->egrTxQConf.txPortRegs[i].descrCounter     = 0x01840000 + offset;
        addrPtr->egrTxQConf.txPortRegs[i].bufferCounter    = 0x018C0000 + offset;

        if (PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_DXCH_XCAT_E)
        {
            addrPtr->egrTxQConf.txPortRegs[i].wdTxFlush = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
            addrPtr->egrTxQConf.txPortRegs[i].l1OverheadConfig = 0x01A80000 + offset;
        }
        else
        {
            addrPtr->egrTxQConf.txPortRegs[i].wdTxFlush = 0x01A80000 + offset;
            addrPtr->egrTxQConf.txPortRegs[i].l1OverheadConfig = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        }

    }

    /* Tail drop Limits: 4 profiles, 0x01800030 */
    /* see also 0x01800080 */
    for (j = 0; j < PRV_CPSS_DXCH_PROFILES_NUM_CNS; j++)
    {

        addrPtr->egrTxQConf.setsConfigRegs.portLimitsConf[j] = 0x01800030 + j * 4;

        for (i = 0; i < 8; i++)
        {
            offset = j * 0x0400 + i * 0x08;
            addrPtr->egrTxQConf.setsConfigRegs.tcDp0Red[i][j] = 0x01940000 + offset;
            addrPtr->egrTxQConf.setsConfigRegs.tcDp1Red[i][j] = 0x01940004 + offset;
        }
    }

    for (i = 0; i < 8; i++)
    {
        offset = i * 0x8000;
        addrPtr->egrTxQConf.txCpuRegs.prioTokenBuck[i]     = 0x01AC7E00 + offset;
        addrPtr->egrTxQConf.txCpuRegs.prioTokenBuckLen[i]  = 0x01AC7E00 + offset;
        addrPtr->egrTxQConf.txCpuRegs.tcDescrCounter[i]    = 0x01887E00 + offset;
        addrPtr->egrTxQConf.txCpuRegs.tcBufferCounter[i]   = 0x01907E00 + offset;
    }

    addrPtr->egrTxQConf.txCpuRegs.tokenBuck = 0x01AC7E08;
    addrPtr->egrTxQConf.txCpuRegs.tokenBuckLen = 0x01AC7E0C;
    addrPtr->egrTxQConf.txCpuRegs.txConfig = 0x01807E80;
    addrPtr->egrTxQConf.txCpuRegs.descrCounter = 0x01847E00;
    addrPtr->egrTxQConf.txCpuRegs.bufferCounter = 0x018C7E00;

    if (PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        addrPtr->egrTxQConf.txCpuRegs.wdTxFlush = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->egrTxQConf.txCpuRegs.l1OverheadConfig = 0x01A87E00;
    }
    else
    {
        addrPtr->egrTxQConf.txCpuRegs.wdTxFlush = 0x01A87E00;
        addrPtr->egrTxQConf.txCpuRegs.l1OverheadConfig = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    }


    addrPtr->egrTxQConf.txQueueConfig = 0x01800000;
    addrPtr->egrTxQConf.txQMcFifoEccConfig = 0x0180000C;
    addrPtr->egrTxQConf.trStatSniffAndStcReg = 0x01800020;
    addrPtr->egrTxQConf.txQueueMiscCtrl = 0x018001B0;

    /* outBytePkts is amount of BC packets both TWIST and Cheetah */
    for (i = 0; i < 2; i++)
    {
        offset = i * 0x20;
        addrPtr->egrTxQConf.txQCountSet[i].txQConfig       = 0x01800140 + offset;
        addrPtr->egrTxQConf.txQCountSet[i].outUcPkts       = 0x01B40144 + offset;
        addrPtr->egrTxQConf.txQCountSet[i].outMcPkts       = 0x01B40148 + offset;
        addrPtr->egrTxQConf.txQCountSet[i].outBytePkts     = 0x01B4014C + offset;
        addrPtr->egrTxQConf.txQCountSet[i].brgEgrFiltered  = 0x01B40150 + offset;
        addrPtr->egrTxQConf.txQCountSet[i].txQFiltered     = 0x01B40154 + offset;
        addrPtr->egrTxQConf.txQCountSet[i].outCtrlPkts     = 0x01B40158 + offset;
    }

    addrPtr->egrTxQConf.sniffTailDropCfgReg = 0x01800024;
    addrPtr->egrTxQConf.txQueueConfigExt = 0x01800004;
    addrPtr->egrTxQConf.tcDp0CscdDataRemap = 0x01800018;
    addrPtr->egrTxQConf.tcDp1CscdDataRemap = 0x0180001C;

    addrPtr->egrTxQConf.totalBufferLimitConfig = 0x01800084;
}

/*******************************************************************************
* cheetahIntrRegsInit
*
* DESCRIPTION:
*       This function initializes the interrupt registers struct
*       for devices that belong to REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with interrupt regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetahIntrRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    /* for interrupt connection via PCI */
    addrPtr->interrupts.intSumCauseBookmark = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
    addrPtr->interrupts.intSumCauseType= PRV_CPSS_SW_TYPE_WRONLY_CNS ;
    addrPtr->interrupts.intSumCauseSize= 3;
    addrPtr->interrupts.intSumCause    = 0x0114;  /* for interrupt connection via PCI */
    addrPtr->interrupts.intSumMask     = 0x0118; /* for interrupt connection via PCI */

    /* in the Cheetah the Bridge Interrupts
       are moved to FDB interrupt registers */
    addrPtr->interrupts.ethBrgCause    = 0x06000018;
    addrPtr->interrupts.ethBrgMask     = 0x0600001C;
    addrPtr->interrupts.bmIntCause     = 0x03000040;
    addrPtr->interrupts.bmIntMask      = 0x03000044;
    addrPtr->interrupts.miscIntCause   = 0x0038;
    addrPtr->interrupts.miscIntMask    = 0x003C;
    addrPtr->interrupts.txqIntCause    = 0x01800118;
    addrPtr->interrupts.txqIntMask     = 0x0180011C;
    addrPtr->interrupts.rxDmaIntCause  = 0x280C;
    addrPtr->interrupts.rxDmaIntMask   = 0x2814;
    addrPtr->interrupts.txDmaIntCause  = 0x2810;
    addrPtr->interrupts.txDmaIntMask   = 0x2818;
}

/*******************************************************************************
* cheetahI2CRegsInit
*
* DESCRIPTION:
*       This function initializes the I2C registers struct
*       for devices that belong to REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with i2c regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetahI2CRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    addrPtr->swi2cRegBookmark   = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
    addrPtr->swi2cRegType      = PRV_CPSS_SW_TYPE_WRONLY_CNS;
    addrPtr->swi2cRegSize      = sizeof(addrPtr->i2cReg) / sizeof(GT_U32);
    addrPtr->i2cReg.slaveAddr          = 0x00400000;
    addrPtr->i2cReg.slaveAddrExtend    = 0x00400010;
    addrPtr->i2cReg.data               = 0x00400004;
    addrPtr->i2cReg.ctrl               = 0x00400008;
    addrPtr->i2cReg.status             = 0x0040000c;
    addrPtr->i2cReg.baudRate           = 0x0040000C;
    addrPtr->i2cReg.softReset          = 0x0040001C;
    addrPtr->i2cReg.serInitCtrl        = 0x0010;
    addrPtr->i2cReg.serFinishAddr      = 0x0014;
}

/*******************************************************************************
* cheetaSdmaRegsInit
*
* DESCRIPTION:
*       This function initializes the SDMA registers struct
*       for devices that belong to REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with sdma regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetaSdmaRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U8   i;
    GT_U32  offset;

    addrPtr->sdmaRegs.sdmaCfgReg           = 0x00002800;
    addrPtr->sdmaRegs.rxQCmdReg            = 0x00002680;
    addrPtr->sdmaRegs.txQCmdReg            = 0x00002868;
    addrPtr->sdmaRegs.rxDmaResErrCnt[0]    = 0x00002860;
    addrPtr->sdmaRegs.rxDmaResErrCnt[1]    = 0x00002864;
    addrPtr->sdmaRegs.txQFixedPrioConfig   = 0x00002870;

    for (i = 0; i < 8; i++)
    {
        offset = i * 0x10;
        addrPtr->sdmaRegs.rxDmaCdp[i]          = 0x260C + offset;
        addrPtr->sdmaRegs.txQWrrPrioConfig[i]  = 0x2708 + offset;
        offset = i * 4;
        addrPtr->sdmaRegs.rxDmaPcktCnt[i]      = 0x2820 + offset;
        addrPtr->sdmaRegs.rxDmaByteCnt[i]      = 0x2840 + offset;
        addrPtr->sdmaRegs.txDmaCdp[i]          = 0x26C0 + offset;

    }
}

/*******************************************************************************
* cheetahSerdesRegsAlloc
*
* DESCRIPTION:
*       This function allocates memory for SERDES registers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       GT_OK               - on success, or
*       GT_OUT_OF_CPU_MEM   - om malloc failed
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS cheetahSerdesRegsAlloc
(
    IN GT_U32          devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32  size;
    GT_U32  numOfSerdes;

    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        numOfSerdes = PRV_CPSS_XCAT_SERDES_NUM_CNS;
    }
    else if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {
        numOfSerdes = PRV_CPSS_XCAT2_SERDES_NUM_CNS;
    }
    else if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        numOfSerdes = PRV_CPSS_LION_SERDES_NUM_CNS;
    }
    else
    {
        addrPtr->serdesConfig = NULL;
        addrPtr->serdesConfigRegsBookmark   = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
        addrPtr->serdesConfigRegsType       = PRV_CPSS_SW_TYPE_PTR_FIELD_CNS;
        addrPtr->serdesConfigRegsSize       = 0;
        return GT_OK;
    }

    size = sizeof(struct _lpSerdesConfig) * numOfSerdes;
    addrPtr->serdesConfig = allocAddressesMemory(size);
    if (addrPtr->serdesConfig == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    addrPtr->serdesConfigRegsBookmark   = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
    addrPtr->serdesConfigRegsType       = PRV_CPSS_SW_TYPE_PTR_FIELD_CNS;
    addrPtr->serdesConfigRegsSize       = size;

    return GT_OK;
}


/*******************************************************************************
* cheetahMacRegsAlloc
*
* DESCRIPTION:
*       This function allocates memory for MAC related registers struct
*       for devices that belong to REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       GT_OK               - on success, or
*       GT_OUT_OF_CPU_MEM   - om malloc failed
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS cheetahMacRegsAlloc
(
    IN GT_U32          devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32  size;

    size = sizeof(struct _perPortRegs) * PRV_CPSS_PP_MAC(devNum)->numOfPorts;
    addrPtr->macRegs.perPortRegs = allocAddressesMemory(size);
    if (addrPtr->macRegs.perPortRegs == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    return GT_OK;
}

/*******************************************************************************
* cheetahMacRegsInit
*
* DESCRIPTION:
*       This function initializes MAC related registers struct
*       for devices that belong to REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetahMacRegsInit
(
    IN GT_U32          devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U8   i;
    GT_U32  size;
    GT_U32  lane;
    GT_U32  offset;

    size = sizeof(struct _perPortRegs) * PRV_CPSS_PP_MAC(devNum)->numOfPorts;

    addrPtr->macRegs.swPerPortRegsBookmark = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
    addrPtr->macRegs.swPerPortRegsType     = PRV_CPSS_SW_TYPE_PTR_FIELD_CNS;
    addrPtr->macRegs.swPerPortRegsSize     = size;

    addrPtr->macRegs.srcAddrMiddle = 0x04004024;
    addrPtr->macRegs.srcAddrHigh   = 0x04004028;

    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->haRegs.cscdHeadrInsrtConf,0x07800004);

    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->haRegs.eggrDSATagTypeConf,0x07800020);

    addrPtr->addrSpace.buffMemWriteControl = 0x06800000;
    addrPtr->addrSpace.buffMemBank0Write = 0x06900000;
    addrPtr->addrSpace.buffMemBank1Write = 0x06980000;

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        addrPtr->addrSpace.buffMemClearType  = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    }
    else
    {
        addrPtr->addrSpace.buffMemClearType  = 0x07800010;
    }

    /* Mac counters Control registers   */
    for(i = 0; i < 4; i++)
    {
        addrPtr->macRegs.macCountersCtrl[i] = 0x04004020 + (i * 0x800000) ;
        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            /* in lion the GE ports uses the same counters mechanism as the
               XG ports ! */
            addrPtr->macRegs.macCountersCtrl[i] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        }
    }

    /* HyperG.Stack Ports MIB Counters and XSMII Configuration Register */
    addrPtr->macRegs.hgXsmiiCountersCtr = 0x01800180;

    /* per-port registers   */
    for (i = 0; (i < PRV_CPSS_PP_MAC(devNum)->numOfPorts); i++)
    {
        offset = i * 0x100;

        /* serialParameters - per port (0..23) */
        addrPtr->macRegs.perPortRegs[i].serialParameters=
            ((i < 24)
             ? (0x0A800014 + offset)
             : PRV_CPSS_SW_PTR_ENTRY_UNUSED) ;
        /* MIB MAC counters */

        addrPtr->macRegs.perPortRegs[i].macCounters =
            ((i < 24)
             ? (0x04010000 + ((i / 6) * 0x0800000) + ((i % 6) * 0x0080))
             : (0x01C00000 + ((i - 24) * 0x040000))) ;

        addrPtr->macRegs.perPortRegs[i].macCaptureCounters =
            ((i < 24)
             ? (0x04010300 + ((i / 6) * 0x0800000))
             : (0x01C00080 + ((i - 24) * 0x040000))) ;

        /* Mac control registers    */
        addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_FE_E].macCtrl =
            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl =
                 0x0A800000 + offset;

        addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_FE_E].macCtrl1 =
            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl1 =
                0x0A800004 + offset;

        addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_FE_E].macCtrl2 =
            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl2 =
                0x0A800008 + offset;

        if(i >= 24)
        {
            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl =
                    0x0A800000 + offset;
            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl1 =
                    0x0A800004 + offset;
            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl2 =
                    0x0A800008 + offset;
        }
        else
        {
            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl =
                    PRV_CPSS_SW_PTR_ENTRY_UNUSED;
            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl1 =
                    PRV_CPSS_SW_PTR_ENTRY_UNUSED;
            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl2 =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        }

        addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XLG_E].macCtrl =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XLG_E].macCtrl1 =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XLG_E].macCtrl2 =
            PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XLG_E].macCtrl3 =
            PRV_CPSS_SW_PTR_ENTRY_UNUSED;


        /* Mac status registers     */
        addrPtr->macRegs.perPortRegs[i].macStatus =
            ((i < 24)
             ? (0x0A800010 + offset)
             : (0x0A80180C + offset - (24 * 0x100))) ;
        addrPtr->macRegs.perPortRegs[i].autoNegCtrl =
            ((i < 24)
             ? (0x0A80000C + offset)
             : PRV_CPSS_SW_PTR_ENTRY_UNUSED) ;
        addrPtr->macRegs.perPortRegs[i].serdesCnfg =
            ((i < 24)
             ? (0x0A800028 + offset)
             : PRV_CPSS_SW_PTR_ENTRY_UNUSED) ;

        /* PRBS registers */
        addrPtr->macRegs.perPortRegs[i].prbsCheckStatus = 0x0A800038 + offset;
        addrPtr->macRegs.perPortRegs[i].prbsErrorCounter = 0x0A80003C + offset;

        addrPtr->macRegs.perPortRegs[i].xgMibCountersCtrl = PRV_CPSS_SW_PTR_ENTRY_UNUSED;

        for(lane = 0; lane < 4; lane++)
        {
            addrPtr->macRegs.perPortRegs[i].serdesPowerUp1[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesPowerUp2[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesReset[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesCommon[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].laneConfig0[lane] =
                 PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].laneConfig1[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].disparityErrorCounter[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesTxSyncConf[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesSpeed1[lane] =
                 PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesSpeed2[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesSpeed3[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesTxConfReg1[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesTxConfReg2[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesRxConfReg1[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesRxConfReg2[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesDtlConfReg2[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesDtlConfReg3[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesDtlConfReg5[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesMiscConf[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesReservConf[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesIvrefConf1[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesIvrefConf2[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesProcessMon[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesCalibConf1[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesCalibConf2[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].prbsErrorCounterLane[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].laneStatus[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].cyclicData[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        }

        addrPtr->macRegs.perPortRegs[i].xgGlobalConfReg0 =
                 PRV_CPSS_SW_PTR_ENTRY_UNUSED;

        addrPtr->macRegs.perPortRegs[i].hxPortConfig0[0]  =
            PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->macRegs.perPortRegs[i].hxPortConfig0[1]  =
            PRV_CPSS_SW_PTR_ENTRY_UNUSED;

        addrPtr->macRegs.perPortRegs[i].hxPortConfig1[0]  =
            PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->macRegs.perPortRegs[i].hxPortConfig1[1]  =
            PRV_CPSS_SW_PTR_ENTRY_UNUSED;

        addrPtr->macRegs.perPortRegs[i].ccfcFcTimerBaseGig =
            PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->macRegs.perPortRegs[i].ccfcFcTimerBaseXg =
            PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    }

    /* set the Ethernet CPU port registers -- port 63 */
    offset = 63 * 0x100;
    addrPtr->macRegs.cpuPortRegs.macStatus         = 0x0A800010 + offset;
    addrPtr->macRegs.cpuPortRegs.macCounters = 0x00000060;
    addrPtr->macRegs.cpuPortRegs.autoNegCtrl       = 0x0A80000C + offset;
    addrPtr->macRegs.cpuPortRegs.serdesCnfg = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->macRegs.cpuPortRegs.macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl     = 0x0A800000 + offset;
    addrPtr->macRegs.cpuPortRegs.macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl1    = 0x0A800004 + offset;
    addrPtr->macRegs.cpuPortRegs.macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl2    = 0x0A800008 + offset;
    addrPtr->macRegs.cpuPortRegs.serialParameters  = 0x0A800014 + offset;

    /* Config HX ports */
    if ((PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98DX249_CNS) ||
        (PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98DX269_CNS) ||
        (PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98DX169_CNS))
    {
        addrPtr->macRegs.perPortRegs->hxPortConfig0[0]  = 0x0A803900; /* port 25 */
        addrPtr->macRegs.perPortRegs->hxPortConfig0[1]  = 0x0A803A00; /* port 26 */

        addrPtr->macRegs.perPortRegs->hxPortConfig1[0]  = 0x0A803904; /* port 25 */
        addrPtr->macRegs.perPortRegs->hxPortConfig1[1]  = 0x0A803A04; /* port 26 */
    }

    /* HXPorts Global Configuration register - relevant only for DX269 */
    if (PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98DX269_CNS)
    {
        addrPtr->macRegs.hxPortGlobalConfig = 0x0A802000;
    }
    else
    {
        addrPtr->macRegs.hxPortGlobalConfig = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    }


    for(i=0 ; i < 6 ; i++)
    {
        addrPtr->macRegs.QSGMIIControlAndStatus[i].QSGMIIControlRegister0 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->macRegs.QSGMIIControlAndStatus[i].QSGMIIStatusRegister   = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->macRegs.QSGMIIControlAndStatus[i].QSGMIIPRBSErrorCounter = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->macRegs.QSGMIIControlAndStatus[i].QSGMIIControlRegister1 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->macRegs.QSGMIIControlAndStatus[i].QSGMIIControlRegister2 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    }

}

/*******************************************************************************
* cheetahLedRegsInit
*
* DESCRIPTION:
*       This function initializes the Led registers struct for devices that
*       belong to REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The PP's device number to init the struct for.
*       addrPtr     - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with SMI regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetahLedRegsInit
(
    IN GT_U32                               devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    addrPtr->ledRegs.ledControl[0] = 0x04004100 ; /*ports 0-11,CPU,26*/

    addrPtr->ledRegs.ledClass[0][0] = 0x04004108; /*ports 0-11, class 0*/
    addrPtr->ledRegs.ledClass[0][1] = 0x04004108; /*ports 0-11, class 1*/
    addrPtr->ledRegs.ledClass[0][2] = 0x04804108; /*ports 0-11, class 2*/
    addrPtr->ledRegs.ledClass[0][3] = 0x04804108; /*ports 0-11, class 3*/
    addrPtr->ledRegs.ledClass[0][4] = 0x0400410C; /*ports 0-11, class 4*/
    addrPtr->ledRegs.ledClass[0][5] = 0x0480410C; /*ports 0-11, class 5*/
    addrPtr->ledRegs.ledClass[0][6] = 0x04804100; /*ports 0-11, class 6*/
    /* OUT of table 0x05805100 - ports 24-25, classes 10-11*/

    addrPtr->ledRegs.ledGroup[0][0] = 0x04004104; /* ports 0-11, group 0*/
    addrPtr->ledRegs.ledGroup[0][1] = 0x04004104; /* ports 0-11, group 1*/
    addrPtr->ledRegs.ledGroup[0][2] = 0x04804104; /* ports 0-11, group 2*/
    addrPtr->ledRegs.ledGroup[0][3] = 0x04804104; /* ports 0-11, group 3*/


    if (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_LION_E)
    {
        addrPtr->ledRegs.ledControl[1] = 0x05004100 ; /*ports 12-23,24,25*/

        addrPtr->ledRegs.ledClass[1][0] = 0x05004108; /*ports 12-23, class 0*/
        addrPtr->ledRegs.ledClass[1][1] = 0x05004108; /*ports 12-23, class 1*/
        addrPtr->ledRegs.ledClass[1][2] = 0x05804108; /*ports 12-23, class 2*/
        addrPtr->ledRegs.ledClass[1][3] = 0x05804108; /*ports 12-23, class 3*/
        addrPtr->ledRegs.ledClass[1][4] = 0x0500410C; /*ports 12-23, class 4*/
        addrPtr->ledRegs.ledClass[1][5] = 0x0580410C; /*ports 12-23, class 5*/
        addrPtr->ledRegs.ledClass[1][6] = 0x05804100; /*ports 12-23, class 6*/

        addrPtr->ledRegs.ledGroup[1][0] = 0x05004104; /* ports 12-23, group 0*/
        addrPtr->ledRegs.ledGroup[1][1] = 0x05004104; /* ports 12-23, group 1*/
        addrPtr->ledRegs.ledGroup[1][2] = 0x05804104; /* ports 12-23, group 2*/
        addrPtr->ledRegs.ledGroup[1][3] = 0x05804104; /* ports 12-23, group 3 */

        addrPtr->ledRegs.ledXgClass04Manipulation[0]    = 0x4005100;
        addrPtr->ledRegs.ledXgClass59Manipulation[0]    = 0x4005104;
        addrPtr->ledRegs.ledHyperGStackDebugSelect[0]   = 0x4005110;
        addrPtr->ledRegs.ledHyperGStackDebugSelect1[0]  = 0x4005114;
        addrPtr->ledRegs.ledXgClass1011Manipulation[0]  = 0x4805100;
        addrPtr->ledRegs.ledXgGroup01Configuration[0]   = 0x4805104;

        addrPtr->ledRegs.ledXgClass04Manipulation[1]    = 0x5005100;
        addrPtr->ledRegs.ledXgClass59Manipulation[1]    = 0x5005104;
        addrPtr->ledRegs.ledXgClass1011Manipulation[1]  = 0x5805100;
        addrPtr->ledRegs.ledXgGroup01Configuration[1]   = 0x5805104;
    }
}

/*******************************************************************************
* cheetahSmiRegsAlloc
*
* DESCRIPTION:
*       This function allocates memory for the Smi
*       Configuration registers struct for devices that belong to
*       REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       GT_OK               - on success, or
*       GT_OUT_OF_CPU_MEM   - om malloc failed
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS cheetahSmiRegsAlloc
(
    IN GT_U32          devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32  size;

    size = sizeof(GT_U32) * PRV_CPSS_PP_MAC(devNum)->numOfPorts;
    addrPtr->smiRegs.smi10GePhyConfig0 = allocAddressesMemory(size);
    addrPtr->smiRegs.smi10GePhyConfig1 = allocAddressesMemory(size);
    if ( (addrPtr->smiRegs.smi10GePhyConfig0 == NULL) ||
         (addrPtr->smiRegs.smi10GePhyConfig1 == NULL) )
    {
        return GT_OUT_OF_CPU_MEM;
    }

    addrPtr->smiRegs.swSmi10GePhyCfg0Bookmark  = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
    addrPtr->smiRegs.swSmi10GePhyCfg0Type      = PRV_CPSS_SW_TYPE_PTR_FIELD_CNS;
    addrPtr->smiRegs.swSmi10GePhyCfg0Size      = size;
    addrPtr->smiRegs.swSmi10GePhyCfg1Bookmark  = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
    addrPtr->smiRegs.swSmi10GePhyCfg1Type      = PRV_CPSS_SW_TYPE_PTR_FIELD_CNS;
    addrPtr->smiRegs.swSmi10GePhyCfg1Size      = size;

    return GT_OK;
}

/*******************************************************************************
* cheetahSmiRegsInit
*
* DESCRIPTION:
*       This function initializes the Serial Management Interface registers
*       struct for devices that belong to REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with SMI regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetahSmiRegsInit
(
    IN GT_U32          devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
   GT_U32 i;
   GT_U32 offset;

   addrPtr->smiRegs.smiControl     = 0x04004054; /* ports 0-11*/
   /* out of data                 0x05004054     ports 12-23*/
   addrPtr->smiRegs.smiPhyAddr     = 0x04004030; /* ports 0-5*/
   /* out of data                 0x04804030     ports 6-11*/
   /* out of data                 0x05004030     ports 12-17*/
   /* out of data                 0x05804030     ports 18-23*/
   
   addrPtr->smiRegs.smi10GeAcTiming= 0x018001f8;

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
       if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
       {
            addrPtr->smiRegs.smi10GeControl = 0x07E40000;
            addrPtr->smiRegs.smi10GeAddr    = 0x07E40008;
       }
       else
       {
            addrPtr->smiRegs.smi10GeControl = 0x01cc0000;
            addrPtr->smiRegs.smi10GeAddr    = 0x01cc0008;
       }
    }
    else
    {
        /* not in the TXQ unit any more --> but in unit MPPM/XSMI_Configuration */
        addrPtr->smiRegs.smi10GeControl = 0x06E40000;
        addrPtr->smiRegs.smi10GeAddr    = 0x06E40008;
    }

   for(i=0; i<PRV_CPSS_PP_MAC(devNum)->numOfPorts; i++)
   {
        if(PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[i].portType < PRV_CPSS_PORT_XG_E ||
           PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            addrPtr->smiRegs.smi10GePhyConfig0[i] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
            addrPtr->smiRegs.smi10GePhyConfig1[i] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        }
        else
        {
            offset = 0x100 * i;
            addrPtr->smiRegs.smi10GePhyConfig0[i] = 0x0a80001c + offset;
            addrPtr->smiRegs.smi10GePhyConfig1[i] = 0x0a800020 + offset;
        }
   }

   if (PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_DXCH_XCAT_E)
   {
       addrPtr->smiRegs.lms0MiscConfig = 0x04004200;

       if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
       {
           addrPtr->smiRegs.lms1MiscConfig = PRV_CPSS_SW_PTR_ENTRY_UNUSED ;
       }
       else
       {
           addrPtr->smiRegs.lms1MiscConfig = 0x05004200;
       }
   }
}

/*******************************************************************************
* cheetahIpRegsInit
*
* DESCRIPTION:
*       This function initializes the IP registers struct for
*       devices that belong to REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with IPv4 regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetahIpRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{

    addrPtr->ipRegs.ctrlReg0 = 0x0C000040;

    addrPtr->haRegs.hdrAltGlobalConfig = 0x07800100;
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->haRegs.routerHdrAltEnMacSaModifyReg,0x07800104);
    addrPtr->haRegs.routerHdrAltMacSaModifyMode[0] = 0x07800120;
    addrPtr->haRegs.routerHdrAltMacSaModifyMode[1] = 0x07800124;
    addrPtr->haRegs.routerMacSaBaseReg[0] = 0x07800108;
    addrPtr->haRegs.routerMacSaBaseReg[1] = 0x0780010C;
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->haRegs.routerDevIdModifyEnReg,0x07800110);

    addrPtr->ipRegs.ucRoutingEngineConfigurationReg = PRV_CPSS_SW_PTR_ENTRY_UNUSED;

    addrPtr->ipRegs.routerPerPortSipSaEnable0 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
}

/*******************************************************************************
* cheetahTrafCndRegsInit
*
* DESCRIPTION:
*       This function initializes the Traffic Control block registers
*       struct for devices that belong to REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetahTrafCndRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    /*********************/
    /* policer registers */
    /*********************/
    /* policer global register */
    addrPtr->PLR[0].policerControlReg = 0x0C000000;
}

/*******************************************************************************
* cheetaPclRegsInit
*
* DESCRIPTION:
*       This function initializes the Internal InLif Tables registers
*       struct for devices that belong to REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
static void cheetaPclRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32 i;

    /* Cheetah - PCL registers - temporary ommited */
    addrPtr->pclRegs.PclBookmark = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
    addrPtr->pclRegs.PclType =  PRV_CPSS_SW_TYPE_WRONLY_CNS;
    addrPtr->pclRegs.PclSize = 57;
    addrPtr->pclRegs.control = 0x0B800000;
    for (i = 0; (i < 12); i++)
    {
        addrPtr->pclRegs.tcamWriteData[i] = 0x0B800100 + (4 * i);
    }

    addrPtr->pclRegs.tcamReadMaskBase     = 0x0B810000;
    addrPtr->pclRegs.tcamReadPatternBase  = 0x0B818000;
    addrPtr->pclRegs.tcamOperationTrigger = 0x0B800130;

    addrPtr->pclRegs.udbConfigKey0bytes0_2 = 0x0B800020;
    addrPtr->pclRegs.udbConfigKey0bytes3_5 = 0x0B800024;
    addrPtr->pclRegs.udbConfigKey1bytes0_2 = 0x0B800028;
    addrPtr->pclRegs.udbConfigKey1bytes3_5 = 0x0B80002C;
    addrPtr->pclRegs.udbConfigKey2bytes0_2 = 0x0B800030;
    addrPtr->pclRegs.udbConfigKey3bytes0_2 = 0x0B800034;
    addrPtr->pclRegs.udbConfigKey4bytes0_1 = 0x0B800038;
    addrPtr->pclRegs.udbConfigKey5bytes0_2 = 0x0B80003C;

    addrPtr->pclRegs.ruleMatchCounersBase = 0x0B801000;

    /* to override for next device versions */

    addrPtr->pclRegs.tcamOperationParam1 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.tcamOperationParam2 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.tcamReadMaskControlBase = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.tcamReadPatternControlBase = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.actionTableBase = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.epclGlobalConfig = PRV_CPSS_SW_PTR_ENTRY_UNUSED;

    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclCfgTblAccess,PRV_CPSS_SW_PTR_ENTRY_UNUSED);
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclEnPktNonTs,PRV_CPSS_SW_PTR_ENTRY_UNUSED);
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclEnPktTs,PRV_CPSS_SW_PTR_ENTRY_UNUSED);
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclEnPktToCpu,PRV_CPSS_SW_PTR_ENTRY_UNUSED);
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclEnPktFromCpuData,PRV_CPSS_SW_PTR_ENTRY_UNUSED);
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclEnPktFromCpuControl,PRV_CPSS_SW_PTR_ENTRY_UNUSED);
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclEnPktToAnalyzer,PRV_CPSS_SW_PTR_ENTRY_UNUSED);

    addrPtr->pclRegs.ipclTcpPortComparator0 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.ipclUdpPortComparator0 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.epclTcpPortComparator0 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.epclUdpPortComparator0 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.fastStack = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.loopPortReg = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.cncCountMode = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.cncl2l3IngressVlanCountEnable = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.pclTcamConfig0 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.pclTcamConfig2 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.pclTcamControl = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.pclUDBConfig = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.pclEngineConfig = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.ttiUnitConfig   = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.ttiEngineConfig = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.eplrGlobalConfig = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
}

/*******************************************************************************
* cheetahSflowRegsInit
*
* DESCRIPTION:
*       This function initializes the Internal
*        sFlow registers struct for devices that belong to
*       REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
static void cheetahSflowRegsInit
(
    IN GT_U32          devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32 i;
    GT_U32 offset;

    addrPtr->sFlowRegs.sFlowControl            = 0x0B00001C;

    /* Cheetah sampling to cpu registers */
    addrPtr->sFlowRegs.ingStcTblAccessCtrlReg = 0x0B000038;
    addrPtr->sFlowRegs.ingStcTblWord0ValueReg = 0x0B005000;
    addrPtr->sFlowRegs.ingStcTblWord1ValueReg = 0x0B005004; /* read only */
    addrPtr->sFlowRegs.ingStcTblWord2ValueReg = 0x0B005008;
    /* addrPtr->sFlowRegs.ingStcIntCauseReg      = 0x0B000020; */

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
    /* Cheetah egress sampling to CPU per port registers */
    for (i = 0; i < PRV_CPSS_PP_MAC(devNum)->numOfPorts; i++)
    {
        offset = i * 0x200;
        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            offset = (i & 0xf) * 0x200;

            if((i & 0xf) > 0xb)
            {
                addrPtr->sFlowRegs.egrStcTblWord0[i] =
                addrPtr->sFlowRegs.egrStcTblWord1[i] =
                addrPtr->sFlowRegs.egrStcTblWord2[i] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
                continue;
            }
        }

        addrPtr->sFlowRegs.egrStcTblWord0[i] = 0x01D40000 + offset;
        addrPtr->sFlowRegs.egrStcTblWord1[i] = 0x01D40004 + offset;
        addrPtr->sFlowRegs.egrStcTblWord2[i] = 0x01D40008 + offset;
    }
    }

}

/*******************************************************************************
* cheetaTtiRegsInit
*
* DESCRIPTION:
*       This function initializes the TTI registers
*       struct for devices that belong to REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
static void cheetahTtiRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    addrPtr->ttiRegs.pclIdConfig0        = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->ttiRegs.pclIdConfig1        = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->ttiRegs.ttiIpv4GreEthertype = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->ttiRegs.specialEthertypes   = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->ttiRegs.mplsEthertypes      = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
}

/*******************************************************************************
* cheetaHaRegsInit
*
* DESCRIPTION:
*       This function initializes the HA registers struct for
*       devices that belong to REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with HA regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetahHaRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    addrPtr->haRegs.hdrAltMplsEthertypes = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->haRegs.hdrAltIEthertype = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
}

/*******************************************************************************
* cheetah3CncRegsInit
*
* DESCRIPTION:
*       This function initializes the CNC registers struct for devices that
*       Cheetah 3
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with global regs init
*
* RETURNS:
*       GT_OK               - on success, or
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetah3CncRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    /* global configuration           */
    addrPtr->cncRegs.globalCfg                         = 0x08000000;
    addrPtr->cncRegs.blockUploadTriggers               = 0x08000030;
    addrPtr->cncRegs.clearByReadValueWord0             = 0x08000040;
    addrPtr->cncRegs.clearByReadValueWord1             = 0x08000044;
    /* per couner block configuration */
    addrPtr->cncRegs.blockClientCfgAddrBase             = 0x08001080;
    addrPtr->cncRegs.blockWraparoundStatusAddrBase      = 0x080010A0;
    /* counters in blocks */
    addrPtr->cncRegs.blockCountersAddrBase      = 0x08080000;
}

/*******************************************************************************
* prvCpssDxChHwRegAddrInit
*
* DESCRIPTION:
*       This function initializes the registers struct for cheetah devices.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwRegAddrInit
(
    IN  GT_U32 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC *regsAddrPtr;
    GT_U32       *regsAddrPtr32;
    GT_U32        regsAddrPtr32Size;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    /*Fill the all words in the struch with initial value*/
    regsAddrPtr32 = (GT_U32*)regsAddrPtr;
    regsAddrPtr32Size = sizeof(PRV_CPSS_DXCH_PP_REGS_ADDR_STC) / 4;

    defaultAddressUnusedSet(regsAddrPtr32,regsAddrPtr32Size);

    /*
        Allocation for pointers inside regsAddr.
    */
    if ((cheetahMacRegsAlloc(devNum, regsAddrPtr) != GT_OK)      ||
        (cheetahEgrTxRegsAlloc(devNum, regsAddrPtr) != GT_OK)    ||
        (cheetahBridgeRegsAlloc(devNum, regsAddrPtr) != GT_OK)   ||
        (cheetahBufMngRegsAlloc(devNum, regsAddrPtr) != GT_OK)   ||
        (cheetahSmiRegsAlloc(devNum, regsAddrPtr) != GT_OK) ||
        (cheetahSerdesRegsAlloc(devNum,regsAddrPtr) != GT_OK))
    {
        return GT_OUT_OF_CPU_MEM;
    }

    /*
        Initialization of the struct's fields should be added here.
    */
    cheetahMacRegsInit(devNum, regsAddrPtr);
    cheetahEgrTxRegsInit(devNum, regsAddrPtr);
    cheetahBridgeRegsInit(devNum, regsAddrPtr);
    cheetahBufMngRegsInit(devNum, regsAddrPtr);

    cheetahGlobalRegsInit(regsAddrPtr);
    cheetahIntrRegsInit(regsAddrPtr);
    cheetahI2CRegsInit(regsAddrPtr);
    cheetaSdmaRegsInit(regsAddrPtr);
    cheetahLedRegsInit(devNum, regsAddrPtr);
    cheetahSmiRegsInit(devNum, regsAddrPtr);
    cheetahIpRegsInit(regsAddrPtr);
    cheetahTrafCndRegsInit(regsAddrPtr);

    cheetaPclRegsInit(regsAddrPtr);
    cheetahSflowRegsInit(devNum,regsAddrPtr);
    cheetahTtiRegsInit(regsAddrPtr);
    cheetahHaRegsInit(regsAddrPtr);

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChHwRegAddrRemove
*
* DESCRIPTION:
*       This function release the memory that was allocated by the function
*       prvCpssDxChHwRegAddrInit(...)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwRegAddrRemove
(
    IN  GT_U32 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *regsAddrPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    /* free all the dynamically allocated pointers , and set them to NULL */
    FREE_PTR_IF_NOT_NULL_MAC(regsAddrPtr->bridgeRegs.portControl);
    FREE_PTR_IF_NOT_NULL_MAC(regsAddrPtr->bridgeRegs.vlanRegs.portPrvVlan);
    FREE_PTR_IF_NOT_NULL_MAC(regsAddrPtr->bridgeRegs.macRangeFltrBase);
    FREE_PTR_IF_NOT_NULL_MAC(regsAddrPtr->sFlowRegs.egrStcTblWord0);
    FREE_PTR_IF_NOT_NULL_MAC(regsAddrPtr->sFlowRegs.egrStcTblWord1);
    FREE_PTR_IF_NOT_NULL_MAC(regsAddrPtr->sFlowRegs.egrStcTblWord2);
    FREE_PTR_IF_NOT_NULL_MAC(regsAddrPtr->bufferMng.bufMngPerPort);
    FREE_PTR_IF_NOT_NULL_MAC(regsAddrPtr->egrTxQConf.txPortRegs);
    FREE_PTR_IF_NOT_NULL_MAC(regsAddrPtr->macRegs.perPortRegs);
    FREE_PTR_IF_NOT_NULL_MAC(regsAddrPtr->smiRegs.smi10GePhyConfig0);
    FREE_PTR_IF_NOT_NULL_MAC(regsAddrPtr->smiRegs.smi10GePhyConfig1);
    FREE_PTR_IF_NOT_NULL_MAC(regsAddrPtr->serdesConfig);

    return GT_OK;
}

/*******************************************************************************
* cheetah3BridgeRegsInit
*
* DESCRIPTION:
*       This function initializes the registers bridge struct for devices that
*       belong to REGS_FAMILY_1 registers set. -- cheetah 3
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       addrPtr     - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with bridge regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetah3BridgeRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    addrPtr->bridgeRegs.vntReg.vntOamLoopbackConfReg  = 0x01800094;
    addrPtr->bridgeRegs.vntReg.vntCfmEtherTypeConfReg = 0x0B800810;
    addrPtr->bridgeRegs.vntReg.vntGreEtherTypeConfReg = 0x0B800800;

    /* Ingress Log registers */
    addrPtr->bridgeRegs.ingressLog.etherType   = 0x02040700;
    addrPtr->bridgeRegs.ingressLog.daLow       = 0x02040704;
    addrPtr->bridgeRegs.ingressLog.daHigh      = 0x02040708;
    addrPtr->bridgeRegs.ingressLog.daLowMask   = 0x0204070C;
    addrPtr->bridgeRegs.ingressLog.daHighMask  = 0x02040710;
    addrPtr->bridgeRegs.ingressLog.saLow       = 0x02040714;
    addrPtr->bridgeRegs.ingressLog.saHigh      = 0x02040718;
    addrPtr->bridgeRegs.ingressLog.saLowMask   = 0x0204071C;
    addrPtr->bridgeRegs.ingressLog.saHighMask  = 0x02040720;
    addrPtr->bridgeRegs.ingressLog.macDaResult = 0x02040724;
    addrPtr->bridgeRegs.ingressLog.macSaResult = 0x02040728;
}

/*******************************************************************************
* cheetah3EgrTxRegsInit
*
* DESCRIPTION:
*       This function initializes the Egress and Transmit (Tx) Queue
*       Configuration registers struct for devices that belong to
*       REGS_FAMILY_1 registers set. -- cheetah 3
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetah3EgrTxRegsInit
(
    IN GT_U32          devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32  i;

    if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return;
    }

    addrPtr->egrTxQConf.txQueueDpToCfiPerPortEnReg = 0x018000A4;
    addrPtr->egrTxQConf.txQueueDpToCfiReg = 0x018000A8;

    addrPtr->egrTxQConf.stackRemapPortEnReg = 0x01800098;

    addrPtr->egrTxQConf.txQueueSharedPriorityReg[0] = 0x01800028;
    addrPtr->egrTxQConf.txQueueSharedPriorityReg[1] = 0x0180002C;

    /* allocate array for per port registers */
    for(i = 0; i < 64; i++)
    {
        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            addrPtr->egrTxQConf.secondaryTargetPortMapTbl[i] = 0x01E40000 + (i & 0xf)*0x10;
        }
        else
        {
            addrPtr->egrTxQConf.secondaryTargetPortMapTbl[i] = 0x01E40000 + i*0x10;
            if(i == 27)
            {
                /* no more needed */
                break;
            }
        }
    }

    /* allocate array for per profile registers */
    for(i = 0; i < 8; i++)
    {
        addrPtr->egrTxQConf.stackTcPerProfileRemapTbl[i] = 0x01E80000 + i*4;
    }

    addrPtr->egrTxQConf.totalBuffersCounter         = 0x01800088;
    addrPtr->egrTxQConf.totalDescCounter            = 0x01800120;
    addrPtr->egrTxQConf.multicastDescriptorsCounter = 0x01800124;
    addrPtr->egrTxQConf.snifferDescriptorsCounter   = 0x01800188;
}

/*******************************************************************************
* cheetah3PclRegsInit
*
* DESCRIPTION:
*       This function initializes the PCL registers struct for devices that
*       belong to REGS_FAMILY_1 registers set. -- cheetah 3
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetah3PclRegsInit
(
    IN GT_U32                               devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32  i;
    GT_U32  offset;

    addrPtr->pclRegs.fastStack = 0x0B800818;
    addrPtr->pclRegs.loopPortReg = 0x0B80081C;
    /* overwrites the CH and CH2 address 0x0B800130 */
    addrPtr->pclRegs.tcamOperationTrigger = 0x0B800138;
    addrPtr->pclRegs.tcamOperationParam1 = 0x0B800130;
    addrPtr->pclRegs.tcamOperationParam2 = 0x0B800134;
    addrPtr->pclRegs.tcamReadMaskBase            = 0x0BA00020;
    addrPtr->pclRegs.tcamReadPatternBase         = 0x0BA00000;
    addrPtr->pclRegs.tcamReadMaskControlBase     = 0x0BA00024;
    addrPtr->pclRegs.tcamReadPatternControlBase  = 0x0BA00004;
    addrPtr->pclRegs.actionTableBase             = 0x0B880000;

    /* CNC related registers - CH3 */
    addrPtr->pclRegs.cncCountMode                  = 0x0B800820;
    addrPtr->pclRegs.cncl2l3IngressVlanCountEnable = 0x0B800824;

    /* TCAM low level HW configuration - CH3 and above */
    addrPtr->pclRegs.pclTcamConfig0       = 0x0B820108;
    addrPtr->pclRegs.pclTcamConfig2       = 0x0B820120;
    addrPtr->pclRegs.pclTcamControl       = 0x0B820104;

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E)
    {
        /* Only for DxCh3 devices */

        /* policy TCAM Test */
        for (i = 0; i < 14; i++)
        {
            offset = i * 0x4;
            addrPtr->pclRegs.policyTcamTest.pointerRelReg[i] = 0x0B830000 + offset;
        }

        addrPtr->pclRegs.policyTcamTest.configStatusReg = 0x0B830050;
        addrPtr->pclRegs.policyTcamTest.opcodeCommandReg = 0x0B830054;
        addrPtr->pclRegs.policyTcamTest.opcodeExpectedHitReg = 0x0B830058;
    }
}

/*******************************************************************************
* xCatPclRegsInit
*
* DESCRIPTION:
*       This function initializes XCAT specific PCL regiters addresses
*       to overlap the Cheetah3 registers addresses
*
* INPUTS:
*       addrPtr     - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
static void xCatPclRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    addrPtr->pclRegs.epclGlobalConfig        = 0x0F800000;
        /* bit per port registers */
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclCfgTblAccess,0x0F800004);
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclEnPktNonTs,0x0F800008);
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclEnPktTs,0x0F80000C);
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclEnPktToCpu,0x0F800010);
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclEnPktFromCpuData,0x0F800014);
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclEnPktFromCpuControl,0x0F800018);
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclEnPktToAnalyzer,0x0F80001C);
    /* TCP/UDP port comparators (8 register sequences) */
    addrPtr->pclRegs.ipclTcpPortComparator0  = 0x0C0000A0;
    addrPtr->pclRegs.ipclUdpPortComparator0  = 0x0C0000C0;
    addrPtr->pclRegs.epclTcpPortComparator0  = 0x0F800100;
    addrPtr->pclRegs.epclUdpPortComparator0  = 0x0F800140;
    /* xCat new */
    addrPtr->pclRegs.pclUDBConfig            = 0x0B800014;
    addrPtr->pclRegs.pclEngineConfig         = 0x0B80000C;
    addrPtr->pclRegs.ttiUnitConfig           = 0x0C000000;
    addrPtr->pclRegs.ttiEngineConfig         = 0x0C00000C;
    addrPtr->pclRegs.eplrGlobalConfig        = 0x0F800200;

    /* DSA configuration */
    addrPtr->pclRegs.fastStack               = 0x0C000060;
    addrPtr->pclRegs.loopPortReg             = 0x0C000064;
}


/*******************************************************************************
* xCat2PclRegsInit
*
* DESCRIPTION:
*       This function initializes the PCL registers struct for devices that
*       belong to REGS_FAMILY_1 registers set. -- xCat2.
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCat2PclRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    /* bit per port registers */
    PORTS_BMP_REG_SET_MAC(
        addrPtr->pclRegs.eplrPortBasedMetering,0x0F800230);
}

/*******************************************************************************
* cheetah3PolicerRegsInit
*
* DESCRIPTION:
*       This function initializes the Policer registers struct for devices that
*       belong to REGS_FAMILY_1 registers set. -- cheetah 3
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetah3PolicerRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    /* IPLR Registers */
    addrPtr->PLR[0].policerControlReg            = 0x0C000000;
    addrPtr->PLR[0].policerPortMeteringEnReg[0]  = 0x0C000004;
    addrPtr->PLR[0].policerMRUReg                = 0x0C000010;
    addrPtr->PLR[0].policerErrorReg              = 0x0C000020;
    addrPtr->PLR[0].policerErrorCntrReg          = 0x0C000024;
    addrPtr->PLR[0].policerTblAccessControlReg   = 0x0C000028;
    addrPtr->PLR[0].policerTblAccessDataReg      = 0x0C000030;
    addrPtr->PLR[0].policerRefreshScanRateReg    = 0x0C000050;
    addrPtr->PLR[0].policerRefreshScanRangeReg   = 0x0C000054;
    addrPtr->PLR[0].policerInitialDPReg          = 0x0C000060;
    addrPtr->PLR[0].policerIntCauseReg           = 0x0C000100;
    addrPtr->PLR[0].policerIntMaskReg            = 0x0C000104;
    addrPtr->PLR[0].policerShadowReg             = 0x0C000108;

    /* IPLR Tables */
    addrPtr->PLR[0].policerMeteringCountingTbl   = 0x0C040000;
    addrPtr->PLR[0].policerQosRemarkingTbl       = 0x0C080000;
    addrPtr->PLR[0].policerManagementCntrsTbl    = 0x0C0C0000;
}

/*******************************************************************************
* xCatPolicerRegsInit
*
* DESCRIPTION:
*       This function initializes the Policer registers struct for devices that
*       belong to REGS_FAMILY_1 registers set. -- xCat
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCatPolicerRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    /* IPLR #0 Registers */
    addrPtr->PLR[0].policerControlReg            = 0x0C800000;
    addrPtr->PLR[0].policerPortMeteringEnReg[0]  = 0x0C800008;
    addrPtr->PLR[0].policerPortMeteringEnReg[1]  = 0x0C80000C;
    addrPtr->PLR[0].policerMRUReg                = 0x0C800010;
    addrPtr->PLR[0].ipfixControl                 = 0x0C800014;
    addrPtr->PLR[0].ipfixNanoTimerStampUpload    = 0x0C800018;
    addrPtr->PLR[0].ipfixSecLsbTimerStampUpload  = 0x0C80001C;
    addrPtr->PLR[0].ipfixSecMsbTimerStampUpload  = 0x0C800020;
    addrPtr->PLR[0].ipfixDroppedWaThreshold      = 0x0C800030;
    addrPtr->PLR[0].ipfixPacketWaThreshold       = 0x0C800034;
    addrPtr->PLR[0].ipfixByteWaThresholdLsb      = 0x0C800038;
    addrPtr->PLR[0].ipfixByteWaThresholdMsb      = 0x0C80003C;
    addrPtr->PLR[0].ipfixSampleEntriesLog0       = 0x0C800048;
    addrPtr->PLR[0].ipfixSampleEntriesLog1       = 0x0C80004C;
    addrPtr->PLR[0].policerErrorReg              = 0x0C800050;
    addrPtr->PLR[0].policerErrorCntrReg          = 0x0C800054;
    addrPtr->PLR[0].policerTblAccessControlReg   = 0x0C800070;
    addrPtr->PLR[0].policerTblAccessDataReg      = 0x0C800074;
    addrPtr->PLR[0].policerInitialDPReg          = 0x0C8000C0;
    addrPtr->PLR[0].policerIntCauseReg           = 0x0C800100;
    addrPtr->PLR[0].policerIntMaskReg            = 0x0C800104;
    addrPtr->PLR[0].policerShadowReg             = 0x0C800108;
    addrPtr->PLR[0].policerControl1Reg           = 0x0C800004;
    addrPtr->PLR[0].policerControl2Reg           = 0x0C80002C;
    addrPtr->PLR[0].portMeteringPtrIndexReg      = 0x0C801800;

    /* IPLR #0 Tables */

    /* exclude metering and counting tables from dump because
       policer stage may be without tables */
    addrPtr->PLR[0].policerBookmark = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
    addrPtr->PLR[0].policerType     = PRV_CPSS_SW_TYPE_WRONLY_CNS;
    addrPtr->PLR[0].policerSize     = 3;

    addrPtr->PLR[0].policerMeteringCountingTbl   = 0x0C840000;
    addrPtr->PLR[0].policerQosRemarkingTbl       = 0x0C880000;
    addrPtr->PLR[0].policerManagementCntrsTbl    = 0x0C800500;
    addrPtr->PLR[0].policerCountingTbl           = 0x0C860000;
    addrPtr->PLR[0].policerTimerTbl              = 0x0C800200;
    addrPtr->PLR[0].ipfixWrapAroundAlertTbl      = 0x0C800800;
    addrPtr->PLR[0].ipfixAgingAlertTbl           = 0x0C800900;

    /* IPLR #1 Registers */
    addrPtr->PLR[1].policerControlReg            = 0x0D000000;
    addrPtr->PLR[1].policerPortMeteringEnReg[0]  = 0x0D000008;
    addrPtr->PLR[1].policerPortMeteringEnReg[1]  = 0x0D00000C;
    addrPtr->PLR[1].policerMRUReg                = 0x0D000010;
    addrPtr->PLR[1].ipfixControl                 = 0x0D000014;
    addrPtr->PLR[1].ipfixNanoTimerStampUpload    = 0x0D000018;
    addrPtr->PLR[1].ipfixSecLsbTimerStampUpload  = 0x0D00001C;
    addrPtr->PLR[1].ipfixSecMsbTimerStampUpload  = 0x0D000020;
    addrPtr->PLR[1].ipfixDroppedWaThreshold      = 0x0D000030;
    addrPtr->PLR[1].ipfixPacketWaThreshold       = 0x0D000034;
    addrPtr->PLR[1].ipfixByteWaThresholdLsb      = 0x0D000038;
    addrPtr->PLR[1].ipfixByteWaThresholdMsb      = 0x0D00003C;
    addrPtr->PLR[1].ipfixSampleEntriesLog0       = 0x0D000048;
    addrPtr->PLR[1].ipfixSampleEntriesLog1       = 0x0D00004C;
    addrPtr->PLR[1].policerErrorReg              = 0x0D000050;
    addrPtr->PLR[1].policerErrorCntrReg          = 0x0D000054;
    addrPtr->PLR[1].policerTblAccessControlReg   = 0x0D000070;
    addrPtr->PLR[1].policerTblAccessDataReg      = 0x0D000074;
    addrPtr->PLR[1].policerInitialDPReg          = 0x0D0000C0;
    addrPtr->PLR[1].policerIntCauseReg           = 0x0D000100;
    addrPtr->PLR[1].policerIntMaskReg            = 0x0D000104;
    addrPtr->PLR[1].policerShadowReg             = 0x0D000108;
    addrPtr->PLR[1].policerControl1Reg           = 0x0D000004;
    addrPtr->PLR[1].policerControl2Reg           = 0x0D00002C;
    addrPtr->PLR[1].portMeteringPtrIndexReg      = 0x0D001800;

    /* IPLR #1 Tables */
    /* exclude metering and counting tables from dump because
       policer stage may be without tables */
    addrPtr->PLR[1].policerBookmark = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
    addrPtr->PLR[1].policerType     = PRV_CPSS_SW_TYPE_WRONLY_CNS;
    addrPtr->PLR[1].policerSize     = 3;

    addrPtr->PLR[1].policerMeteringCountingTbl   = 0x0D040000;
    addrPtr->PLR[1].policerQosRemarkingTbl       = 0x0D080000;
    addrPtr->PLR[1].policerManagementCntrsTbl    = 0x0D000500;
    addrPtr->PLR[1].policerCountingTbl           = 0x0D060000;
    addrPtr->PLR[1].policerTimerTbl              = 0x0D000200;
    addrPtr->PLR[1].ipfixWrapAroundAlertTbl      = 0x0D000800;
    addrPtr->PLR[1].ipfixAgingAlertTbl           = 0x0D000900;

    /* EPLR Registers */
    addrPtr->PLR[2].policerControlReg            = 0x03800000;
    addrPtr->PLR[2].policerPortMeteringEnReg[0]  = 0x03800008;
    addrPtr->PLR[2].policerPortMeteringEnReg[1]  = 0x0380000C;
    addrPtr->PLR[2].policerMRUReg                = 0x03800010;
    addrPtr->PLR[2].ipfixControl                 = 0x03800014;
    addrPtr->PLR[2].ipfixNanoTimerStampUpload    = 0x03800018;
    addrPtr->PLR[2].ipfixSecLsbTimerStampUpload  = 0x0380001C;
    addrPtr->PLR[2].ipfixSecMsbTimerStampUpload  = 0x03800020;
    addrPtr->PLR[2].ipfixDroppedWaThreshold      = 0x03800030;
    addrPtr->PLR[2].ipfixPacketWaThreshold       = 0x03800034;
    addrPtr->PLR[2].ipfixByteWaThresholdLsb      = 0x03800038;
    addrPtr->PLR[2].ipfixByteWaThresholdMsb      = 0x0380003C;
    addrPtr->PLR[2].ipfixSampleEntriesLog0       = 0x03800048;
    addrPtr->PLR[2].ipfixSampleEntriesLog1       = 0x0380004C;
    addrPtr->PLR[2].policerErrorReg              = 0x03800050;
    addrPtr->PLR[2].policerErrorCntrReg          = 0x03800054;
    addrPtr->PLR[2].policerTblAccessControlReg   = 0x03800070;
    addrPtr->PLR[2].policerTblAccessDataReg      = 0x03800074;
    addrPtr->PLR[2].policerInitialDPReg          = 0x038000C0;
    addrPtr->PLR[2].policerIntCauseReg           = 0x03800100;
    addrPtr->PLR[2].policerIntMaskReg            = 0x03800104;
    addrPtr->PLR[2].policerShadowReg             = 0x03800108;
    addrPtr->PLR[2].policerControl1Reg           = 0x03800004;
    addrPtr->PLR[2].policerControl2Reg           = 0x0380002C;
    addrPtr->PLR[2].portMeteringPtrIndexReg      = 0x03801800;

    /* EPLR Tables */
    addrPtr->PLR[2].policerMeteringCountingTbl   = 0x03840000;
    addrPtr->PLR[2].policerQosRemarkingTbl       = 0x03880000;
    addrPtr->PLR[2].policerManagementCntrsTbl    = 0x03800500;
    addrPtr->PLR[2].policerCountingTbl           = 0x03860000;
    addrPtr->PLR[2].policerTimerTbl              = 0x03800200;
    addrPtr->PLR[2].ipfixWrapAroundAlertTbl      = 0x03800800;
    addrPtr->PLR[2].ipfixAgingAlertTbl           = 0x03800900;
}


/*******************************************************************************
* cheetah3MacRegsInit
*
* DESCRIPTION:
*       This function initializes MAC related registers struct
*       for devices that belong to REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetah3MacRegsInit
(
    IN GT_U32          devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U8   i;
    GT_U32  lane;
    GT_U32  offset;
    GT_U32  maxLane=4;

    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        maxLane = 4;
    }

    /* Init MAC registers   */
    for (i = 0; (i < PRV_CPSS_PP_MAC(devNum)->numOfPorts); i++)
    {

        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            /* set the the non exists ports of lion with regAdder 'unused' */
            if((i & 0xf) > 0xb)
            {
                continue;
            }
        }


        if(PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[i].portType <
              PRV_CPSS_PORT_XG_E)
        {
            offset = i * 0x400;

            /* serialParameters - per port (0..23) */
            addrPtr->macRegs.perPortRegs[i].serialParameters   = 0x0A800014 + offset;
            addrPtr->macRegs.perPortRegs[i].autoNegCtrl        = 0x0A80000C + offset;
            addrPtr->macRegs.perPortRegs[i].serdesCnfg         = 0x0A800028 + offset;

            /* Mac status registers     */
            addrPtr->macRegs.perPortRegs[i].macStatus = 0x0A800010 + offset;

            /* PRBS registers */
            addrPtr->macRegs.perPortRegs[i].prbsCheckStatus = 0x0A800038 + offset;
            addrPtr->macRegs.perPortRegs[i].prbsErrorCounter = 0x0A80003C + offset;
        }
        /* XG ports */
        else
        {
            offset = i * 0x20000;

            if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
            {
                offset = (i & 0xf) * 0x20000;
            }

            /* MIB MAC counters */
            addrPtr->macRegs.perPortRegs[i].macCounters        = 0x09000000 + offset;
            addrPtr->macRegs.perPortRegs[i].macCaptureCounters = 0x09000080 + offset;

            offset = i * 0x400;

            if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
            {
                offset = (i & 0xf) * 0x400;
            }

            /* Mac control registers    */
            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl =
                0x08800000 + offset;

            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl1 =
                0x08800004 + offset;

            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl2 =
                0x08800008 + offset;

            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl3 =
                0x0880001C + offset;

            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XG_E].miscConfig =
                addrPtr->macRegs.perPortRegs[i].miscConfig = 0x0880002C + offset;

            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XG_E].miscConfig =
                addrPtr->macRegs.perPortRegs[i].miscConfig = 0x08800028 + offset;

            addrPtr->macRegs.perPortRegs[i].xgMibCountersCtrl = 0x08800030 + offset;

            /* Mac status registers     */
            addrPtr->macRegs.perPortRegs[i].macStatus = 0x0880000C + offset;

            addrPtr->macRegs.perPortRegs[i].xgGlobalConfReg0 = 0x08800200 + offset;
            addrPtr->macRegs.perPortRegs[i].xgXpcsGlobalStatus = 0x08800210 + offset;
            if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E)
            {
                addrPtr->macRegs.perPortRegs[i].metalFix = 0x08800240 + offset;
            }

            for(lane = 0; lane < maxLane; lane++)
            {
                offset = i * 0x0400 + lane * 0x044;
                if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
                {
                    offset = (i & 0xf) * 0x0400 + lane * 0x044;
                }

                addrPtr->macRegs.perPortRegs[i].laneConfig0[lane] =
                     0x08800250 + offset;
                addrPtr->macRegs.perPortRegs[i].laneConfig1[lane] =
                     0x08800254 + offset;
                addrPtr->macRegs.perPortRegs[i].disparityErrorCounter[lane] =
                     0x0880026C + offset;
                addrPtr->macRegs.perPortRegs[i].prbsErrorCounterLane[lane] =
                     0x08800270 + offset;
                addrPtr->macRegs.perPortRegs[i].laneStatus[lane] =
                     0x0880025c + offset;
                addrPtr->macRegs.perPortRegs[i].cyclicData[lane] =
                     0x08800284 + offset;
            }
        }

        if ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E) && (i >= 24))
        {/* clear Tri-speed MAC init for XG ports of Cheetah */
            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_FE_E].macCtrl =
                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl =
                    PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_FE_E].macCtrl1 =
                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl1 =
                    PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_FE_E].macCtrl2 =
                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl2 =
                    PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_FE_E].macCtrl3 =
                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl3 =
                    PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        }
        else
        {
            offset = (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
                        ? ((i & 0xf) * 0x400) : (i * 0x400);

            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_FE_E].macCtrl =
                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl =
                    0x0A800000 + offset;

            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_FE_E].macCtrl1 =
                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl1 =
                    0x0A800004 + offset;

            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_FE_E].macCtrl2 =
                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl2 =
                    0x0A800008 + offset;

            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_FE_E].macCtrl3 =
                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl3 =
                    0x0A800048 + offset;
        }

        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            /* the lion get it's configurations in xCatMacRegsInit(..) */
            continue;
        }

        /* SERDES configuration */
        if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E)
        {
            /* Only for DxCh3 devices */
            if(i < 24)
            {
                /* group number = i / 4 */
                offset = (i / 4) * 0x1000;
                addrPtr->macRegs.perPortRegs[i].serdesPowerUp1[0] = 0x09800004 + offset;
                addrPtr->macRegs.perPortRegs[i].serdesPowerUp2[0] = 0x09800008 + offset;
                addrPtr->macRegs.perPortRegs[i].serdesReset[0]    = 0x0980000C + offset;
                addrPtr->macRegs.perPortRegs[i].serdesMetalFix    = 0x09800078 + i*0x400;

                if(PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[i].portType !=
                   PRV_CPSS_PORT_XG_E)
                {
                    offset = i * 0x400;
                    addrPtr->macRegs.perPortRegs[i].serdesCommon[0]        = 0x09800000 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesTxSyncConf[0]    = 0x09800010 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesSpeed1[0]        = 0x09800014 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesSpeed2[0]        = 0x09800018 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesTxConfReg1[0]    = 0x0980001C + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesTxConfReg2[0]    = 0x09800020 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesRxConfReg1[0]    = 0x09800028 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesDtlConfReg5[0]   = 0x0980003C + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesIvrefConf1[0]    = 0x09800044 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesIvrefConf2[0]    = 0x09800048 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesMiscConf[0]      = 0x09800054 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesReservConf[0]    = 0x09800058 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesCalibConf1[0]    = 0x09800060 + offset;

                }
                else /* XG port */
                {
                    for( lane = 0 ; lane < maxLane ; lane++ )
                    {
                        offset = (i - i%4 + lane) * 0x400;
                        addrPtr->macRegs.perPortRegs[i].serdesCommon[lane]     = 0x09800000 + offset;
                        addrPtr->macRegs.perPortRegs[i].serdesTxSyncConf[lane] = 0x09800010 + offset;
                        addrPtr->macRegs.perPortRegs[i].serdesSpeed1[lane]     = 0x09800014 + offset;
                        addrPtr->macRegs.perPortRegs[i].serdesSpeed2[lane]     = 0x09800018 + offset;
                        addrPtr->macRegs.perPortRegs[i].serdesTxConfReg1[lane] = 0x0980001C + offset;
                        addrPtr->macRegs.perPortRegs[i].serdesTxConfReg2[lane] = 0x09800020 + offset;
                        addrPtr->macRegs.perPortRegs[i].serdesRxConfReg1[lane] = 0x09800028 + offset;
                        addrPtr->macRegs.perPortRegs[i].serdesDtlConfReg5[lane]= 0x0980003C + offset;
                        addrPtr->macRegs.perPortRegs[i].serdesIvrefConf1[lane] = 0x09800044 + offset;
                        addrPtr->macRegs.perPortRegs[i].serdesIvrefConf2[lane] = 0x09800048 + offset;
                        addrPtr->macRegs.perPortRegs[i].serdesMiscConf[lane]   = 0x09800054 + offset;
                        addrPtr->macRegs.perPortRegs[i].serdesReservConf[lane] = 0x09800058 + offset;
                        addrPtr->macRegs.perPortRegs[i].serdesCalibConf1[lane] = 0x09800060 + offset;
                    }
                }
            }
            else /* 4 Stack ports 24 - 27 */
            {
                offset = ((i - 24) + 8) * 0x1000;
                addrPtr->macRegs.perPortRegs[i].serdesPowerUp1[0] = 0x09800004 + offset;
                addrPtr->macRegs.perPortRegs[i].serdesPowerUp2[0] = 0x09800008 + offset;
                addrPtr->macRegs.perPortRegs[i].serdesReset[0]    = 0x0980000C + offset;

                for( lane = 0 ; lane < maxLane ; lane++ )
                {
                    offset = (32 + (i - 24) * 4 + lane) * 0x0400;
                    addrPtr->macRegs.perPortRegs[i].serdesSpeed1[lane]     = 0x09800014 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesSpeed2[lane]     = 0x09800018 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesSpeed3[lane]     = 0x0980001C + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesTxConfReg1[lane] = 0x09800020 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesTxConfReg2[lane] = 0x09800024 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesRxConfReg1[lane] = 0x09800028 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesRxConfReg2[lane] = 0x0980002C + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesDtlConfReg2[lane] = 0x0980004c + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesDtlConfReg3[lane] = 0x09800050 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesIvrefConf1[lane]  = 0x09800064 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesProcessMon[lane] = 0x0980006C + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesMiscConf[lane]   = 0x09800074 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesReservConf[lane] = 0x09800078 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesCalibConf1[lane] = 0x09800080 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesCalibConf2[lane] = 0x09800084 + offset;
                }
            }
        }
    }

    /* set the Ethernet CPU port registers -- port 63 */
    offset = 63 * 0x400;
    addrPtr->macRegs.cpuPortRegs.macStatus     = 0x0A800010 + offset;
    addrPtr->macRegs.cpuPortRegs.autoNegCtrl   = 0x0A80000C + offset;

    addrPtr->macRegs.cpuPortRegs.macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl  =  0x0A800000 + offset;
    addrPtr->macRegs.cpuPortRegs.macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl1 =  0x0A800004 + offset;
    addrPtr->macRegs.cpuPortRegs.macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl2 =  0x0A800008 + offset;
    addrPtr->macRegs.cpuPortRegs.macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl3 =  0x0A800048 + offset;

    addrPtr->macRegs.cpuPortRegs.serialParameters = 0x0A800014 + offset;


    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        addrPtr->addrSpace.buffMemBank0Write = 0x10000000;
        /* no second bank */
        addrPtr->addrSpace.buffMemBank1Write = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    }
    else
    {
        addrPtr->addrSpace.buffMemBank1Write   = 0x07100000;
    }

    addrPtr->addrSpace.buffMemWriteControl = 0x07800000;
}

/*******************************************************************************
* cheetah3IpRegsInit
*
* DESCRIPTION:
*       This function initializes the IP registers struct for
*       devices that belong to REGS_FAMILY_1 registers set. -- cheetah 3
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with IP regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetah3IpRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32  i;
    GT_U32  offset;

    /* Router Next Hop Table Age Bits */
    addrPtr->ipRegs.ucMcRouteEntriesAgeBitsBase = 0x02801000;

     /* Tcam bank addresses */
    for (i = 0 ; i <= 3; i++)
        addrPtr->ipRegs.routerTtTcamBankAddr[i] = 0x02A00000 + (i * 0x14000);


    /* IP Hit Log registers */
    for (i = 0; i < 4; i++)
    {
        offset = i * 0x4;
        addrPtr->ipRegs.ipHitLog.dipAddrReg[i]     = 0x02800D00 + offset;
        addrPtr->ipRegs.ipHitLog.dipMaskAddrReg[i] = 0x02800D10 + offset;
        addrPtr->ipRegs.ipHitLog.sipAddrReg[i]     = 0x02800D20 + offset;
        addrPtr->ipRegs.ipHitLog.sipMaskAddrReg[i] = 0x02800D30 + offset;
    }

    addrPtr->ipRegs.ipHitLog.layer4DestPortReg = 0x02800D40;
    addrPtr->ipRegs.ipHitLog.layer4SrcPortReg = 0x02800D44;
    addrPtr->ipRegs.ipHitLog.protocolReg = 0x02800D48;

    for (i = 0; i < 3; i++)
    {
        offset = i * 0x4;
        addrPtr->ipRegs.ipHitLog.dstIpNheEntryReg[i] = 0x02800D50 + offset;
        addrPtr->ipRegs.ipHitLog.srcIpNheEntryReg[i] = 0x02800D5C + offset;
    }

    /* TCAM low level HW configuration */
    addrPtr->ipRegs.ipTcamConfig0       = 0x0280096C;
    addrPtr->ipRegs.ipTcamControl       = 0x02800960;

    /* Router TCAM Test */
    for (i = 0; i < 20; i++)
    {
        offset = i * 0x4;
        addrPtr->ipRegs.routerTcamTest.pointerRelReg[i] = 0x02802000 + offset;
    }

    addrPtr->ipRegs.routerTcamTest.configStatusReg = 0x02802050;
    addrPtr->ipRegs.routerTcamTest.opcodeCommandReg = 0x02802054;
    addrPtr->ipRegs.routerTcamTest.opcodeExpectedHitReg = 0x02802058;

    addrPtr->ipRegs.ucRoutingEngineConfigurationReg = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->ipRegs.routerPerPortSipSaEnable0 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
}

/*******************************************************************************
* cheetah2BridgeRegsInit
*
* DESCRIPTION:
*       This function initializes the registers bridge struct for devices that
*       belong to REGS_FAMILY_1 registers set. -- cheetah 2
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with bridge regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetah2BridgeRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32  j;
    addrPtr->bridgeRegs.nstRegs.brgAccessMatrix = 0x02000600;
    addrPtr->bridgeRegs.nstRegs.brgSecureLevelConfReg      = 0x0600003C;
    addrPtr->bridgeRegs.nstRegs.cpuIngrFrwFltConfReg = 0x0B020000;
    addrPtr->bridgeRegs.nstRegs.netIngrFrwFltConfReg = 0x0B020004;
    addrPtr->bridgeRegs.nstRegs.analyzerIngrFrwFltConfReg = 0x0B020008;
    addrPtr->bridgeRegs.nstRegs.cpuPktsFrwFltConfReg = 0x018001D8;
    addrPtr->bridgeRegs.nstRegs.brgPktsFrwFltConfReg = 0x018001DC;
    addrPtr->bridgeRegs.nstRegs.routePktsFrwFltConfReg = 0x018001E0;
    addrPtr->bridgeRegs.nstRegs.ingressFrwDropCounter = 0x0B02000C;

    addrPtr->bridgeRegs.srcIdReg.srcIdAssignedModeConfReg = 0x02040070;
    addrPtr->bridgeRegs.srcIdReg.srcIdUcEgressFilterConfReg = 0x018001D4;


    addrPtr->bridgeRegs.egressFilterUnregBc = 0x018001f4;


    /* UDP BC destination port config. 12 registers */
    for (j = 0; j < 12; j++)
    {
        addrPtr->bridgeRegs.udpBcDestPortConfigReg[j] = 0x02000500 + j*0x4;
    }

    addrPtr->bridgeRegs.trunkTable = 0x0B009000;


    /* TO ANALYZER VLAN Adding Configuration Registers */
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->haRegs.toAnalyzerVlanAddConfig,0x07800404);

    /* Ingress Analyzer VLAN Tag Configuration Register */
    addrPtr->haRegs.ingAnalyzerVlanTagConfig = 0x07800408;

    /* Egress Analyzer VLAN Tag Configuration Register */
    addrPtr->haRegs.egrAnalyzerVlanTagConfig = 0x0780040C;


    /* Vlan MPU profiles confog reg  */
    addrPtr->bridgeRegs.vlanMruProfilesConfigReg = 0x02000300;
}


/*******************************************************************************
* cheetah2GlobalRegsInit
*
* DESCRIPTION:
*       This function initializes the global registers struct for devices that
*       belong to REGS_FAMILY_1 registers set.-- cheetah 2
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with global regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetah2GlobalRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    addrPtr->globalRegs.fuQBaseAddr = 0xC8;
    addrPtr->globalRegs.fuQControl = 0xCC;
    addrPtr->globalRegs.lastReadTimeStampReg = 0x0000002C;
    addrPtr->globalRegs.sampledAtResetReg = 0x00000028;
}

/*******************************************************************************
* cheetah2BufMngRegsInit
*
* DESCRIPTION:
*       This function initializes the buffer management registers struct
*       for devices that belong to REGS_FAMILY_1 registers set.-- cheetah 2
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with buffer management regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetah2BufMngRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    /* init CPU Code related registers */
    addrPtr->bufferMng.eqBlkCfgRegs.cpuRateLimiterConfBase = 0x0B500000;
    addrPtr->bufferMng.eqBlkCfgRegs.cpuRateLimiterPktCntrBase = 0x0B080004;
    addrPtr->bufferMng.eqBlkCfgRegs.cpuRateLimiterDropCntrReg = 0x0B000068;
    addrPtr->bufferMng.eqBlkCfgRegs.preEgrEngineGlobal = 0x0B050000;
    addrPtr->bufferMng.eqBlkCfgRegs.dupPktsToCpuConfReg = 0x0B000018;
    addrPtr->bufferMng.eqBlkCfgRegs.ieeeResMcstCpuCodeIndexBase = 0x02000804;
    addrPtr->bufferMng.eqBlkCfgRegs.ipv4McstLinkLocalCpuCodeIndexBase =  0x02030800;
    addrPtr->bufferMng.eqBlkCfgRegs.ipv6McstLinkLocalCpuCodeIndexBase =  0x02040800;
    addrPtr->bufferMng.eqBlkCfgRegs.tcpUdpDstPortRangeCpuCodeWord0Base = 0x0B007000;
    addrPtr->bufferMng.eqBlkCfgRegs.tcpUdpDstPortRangeCpuCodeWord1Base = 0x0B007040;
    addrPtr->bufferMng.eqBlkCfgRegs.ipProtCpuCodeBase = 0x0B008000;
    addrPtr->bufferMng.eqBlkCfgRegs.ipProtCpuCodeValid = 0x0B008010;
    addrPtr->bufferMng.bufMngSharedBufConfigReg = 0x03000014;
}

/*******************************************************************************
* cheetah2EgrTxRegsInit
*
* DESCRIPTION:
*       This function initializes the Egress and Transmit (Tx) Queue
*       Configuration registers struct for devices that belong to
*       REGS_FAMILY_1 registers set. -- cheetah 2
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum     - device number
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetah2EgrTxRegsInit
(
    IN GT_U32    devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32  i,j;
    GT_U32  offset;

    if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return;
    }

    /* init all arrays of per profiles registers */
    /* cheetah 2 has extra profiles */
    for (i = PRV_CPSS_DXCH_PROFILES_NUM_CNS, j = 0;
         i < PRV_CPSS_DXCH2_PROFILES_NUM_CNS;
         i++, j++)
    {
        /* WRR output algorithm configured */
        /* via 4 Transmit scheduler  profiles */
        offset = j * 0x10;
        addrPtr->egrTxQConf.txPortRegs[i].wrrWeights0 =   0x01800F00 + offset;
        addrPtr->egrTxQConf.txPortRegs[i].wrrWeights1 =   0x01800F04 + offset;
        addrPtr->egrTxQConf.txPortRegs[i].wrrStrictPrio = 0x01800F08 + offset;
    }

    addrPtr->egrTxQConf.cncCountMode = 0x018000A0;

    /* Tail drop Limits: */
    /* cheetah 2 has extra profiles */
    for (j = PRV_CPSS_DXCH_PROFILES_NUM_CNS;
         j < PRV_CPSS_DXCH2_PROFILES_NUM_CNS;
         j++)
    {
        offset =  (j - PRV_CPSS_DXCH_PROFILES_NUM_CNS)*4;
        addrPtr->egrTxQConf.setsConfigRegs.portLimitsConf[j] = 0x018000C0 + offset;

        for (i = 0; i < 8; i++)
        {
            offset = 0x0400 * (j - PRV_CPSS_DXCH_PROFILES_NUM_CNS) + (i * 0x08);
            addrPtr->egrTxQConf.setsConfigRegs.tcDp0Red[i][j] = 0x01941000 + offset;
            addrPtr->egrTxQConf.setsConfigRegs.tcDp1Red[i][j] = 0x01941004 + offset;
        }
    }

    for (i = 0; i < 2; i++)
    {
        /* egress forwarding dropped packets counter register Cheetah2 only */
        addrPtr->egrTxQConf.txQCountSet[i].egrFrwDropPkts  = 0x01B4015C + i*0x20;
    }

    addrPtr->egrTxQConf.txQueueResSharingAndTunnelEgrFltr = 0x018001F0;
}

/*******************************************************************************
* cheetah2MacRegsInit
*
* DESCRIPTION:
*       This function initializes MAC related registers struct
*       for devices that belong to REGS_FAMILY_1 registers set. - cheetah 2
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetah2MacRegsInit
(
    IN GT_U32          devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->haRegs.invalidCrcModeConfigReg,0x07800400);

    /* the XG port number 27 has different value then the formula of other
       XG ports !!!
    */
    if(PRV_CPSS_PP_MAC(devNum)->numOfPorts >= 28 &&
       PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_LION_E)
    {
        addrPtr->macRegs.perPortRegs[27].macCounters = 0x01DC0000;
    }
}

/*******************************************************************************
* cheetah2Ipv4RegsInit
*
* DESCRIPTION:
*       This function initializes the IPv4 registers struct for
*       devices that belong to REGS_FAMILY_1 registers set. -- cheetah 2
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with IPv4 regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetah2Ipv4RegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    addrPtr->ipv4Regs.ctrlReg0 = 0x02800100;
    addrPtr->ipv4Regs.ctrlReg1 = 0x02800104;
    addrPtr->ipv4Regs.ucEnCtrlReg = 0x02800108;
    addrPtr->ipv4Regs.mcEnCtrlReg = 0x0280010C;
}

/*******************************************************************************
* cheetah2Ipv6RegsInit
*
* DESCRIPTION:
*       This function initializes the IPv6 registers struct for
*       devices that belong to REGS_FAMILY_1 registers set. -- cheetah 2
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with IPv6 regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetah2Ipv6RegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32  i;
    GT_U32  offset;

    addrPtr->ipv6Regs.ctrlReg0 = 0x02800200;
    addrPtr->ipv6Regs.ctrlReg1 = 0x02800204;
    addrPtr->ipv6Regs.ucEnCtrlReg = 0x02800208;
    addrPtr->ipv6Regs.mcEnCtrlReg = 0x0280020C;

    for (i = 0; i < 4 ; i++)
    {
        offset = i * 0x4;
        addrPtr->ipv6Regs.ucScopeTblRegs[i]    = 0x02800278 + offset;
        addrPtr->ipv6Regs.mcScopeTblRegs[i]    = 0x02800288 + offset;
        addrPtr->ipv6Regs.ipScopeRegs[i]       = 0x02800250 + offset;
        addrPtr->ipv6Regs.ipScopeLevel[i]      = 0x02800260 + offset;
    }
}

/*******************************************************************************
* cheetah2IpRegsInit
*
* DESCRIPTION:
*       This function initializes the IP registers struct for
*       devices that belong to REGS_FAMILY_1 registers set. -- cheetah 2
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with IP regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetah2IpRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32  i;
    GT_U32  offset;

    addrPtr->ipRegs.routerGlobalReg = 0x02800000;

    for (i = 0 ; i < 4; i++)
        addrPtr->ipRegs.routerMtuCfgRegs[i] = 0x02800004 + (i * 0x4);

    for (i = 0 ; i <= 4; i++)
        addrPtr->ipRegs.routerTtTcamBankAddr[i] = 0x02A00000 + (i * 0x4000);

    addrPtr->ipRegs.routerTtTcamAccessCtrlReg = 0x0280041C;
    addrPtr->ipRegs.routerTtTcamAccessDataCtrlReg = 0x02800418;

    addrPtr->ipRegs.routerTtTcamAccessDataRegsBase = 0x02800400;

    addrPtr->ipRegs.ucMcRouteEntriesAgeBitsBase = 0x0280100C;

    for (i = 0 ; i < 4; i++)
    {
        offset = i * 0x100;
        addrPtr->ipRegs.routerMngCntSetCfgRegs[i]                  = 0x02800980 + offset;
        addrPtr->ipRegs.routerMngInUcPktCntSet[i]                  = 0x02800900 + offset;
        addrPtr->ipRegs.routerMngMcPktCntSet[i]                    = 0x02800904 + offset;
        addrPtr->ipRegs.routerMngInUcNonRoutedNonExcptPktCntSet[i] = 0x02800908 + offset;
        addrPtr->ipRegs.routerMngInUcNonRoutedExcptPktCntSet[i]    = 0x0280090C + offset;
        addrPtr->ipRegs.routerMngInMcNonRoutedNonExcptPktCntSet[i] = 0x02800910 + offset;
        addrPtr->ipRegs.routerMngInMcNonRoutedExcptCntSet[i]       = 0x02800914 + offset;
        addrPtr->ipRegs.routerMngInUcTrapMrrPktCntSet[i]           = 0x02800918 + offset;
        addrPtr->ipRegs.routerMngInMcTrapMrrPktCntSet[i]           = 0x0280091C + offset;
        addrPtr->ipRegs.routerMngInMcRPFFailCntSet[i]              = 0x02800920 + offset;
        addrPtr->ipRegs.routerMngOutUcPktCntSet[i]                 = 0x02800924 + offset;
    }

    for (i = 0 ; i < MAX_NUM_OF_QOS_PROFILE_CNS ; i++)
    {
        addrPtr->ipRegs.qoSProfile2RouteBlockOffsetMapTable[i] = 0x02800300 + (0x4 * i);
    }

    addrPtr->ipRegs.routerAccessMatrixBase = 0x02800440;

    addrPtr->ipRegs.routerBridgedPktExcptCnt = 0x02800940;
    addrPtr->ipRegs.routerDropCntCfgReg = 0x02800954;
    addrPtr->ipRegs.routerDropCnt = 0x02800950;
    addrPtr->ipRegs.routerInterruptCauseReg = 0x02800D00;
    addrPtr->ipRegs.routerInterruptMaskReg = 0x02800D04;
    addrPtr->ipRegs.routerAdditionalCtrlReg = 0x02800964;

    addrPtr->ipRegs.ucRoutingEngineConfigurationReg = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->ipRegs.routerPerPortSipSaEnable0 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
}

/*******************************************************************************
* cheetah2IpMtRegsInit
*
* DESCRIPTION:
*       This function initializes the IP Multi Traffic registers struct for
*       devices that belong to REGS_FAMILY_1 registers set . -- cheetah 2
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with IPv4 regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetah2IpMtRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32 i;
    GT_U32 offset;

    addrPtr->ipMtRegs.mllGlobalReg = 0x0C800000;
    addrPtr->ipMtRegs.multiTargetRateShapingReg = 0x0C800210;
    addrPtr->ipMtRegs.mtUcSchedulerModeReg = 0x0C800214;
    addrPtr->ipMtRegs.mtTcQueuesSpEnableReg = 0x0C800200;
    addrPtr->ipMtRegs.mtTcQueuesWeightReg = 0x0C800204;
    addrPtr->ipMtRegs.mtTcQueuesGlobalReg = 0x0C800004;
    addrPtr->ipMtRegs.mllMcFifoFullDropCnt = 0x0C800984;
    for (i = 0 ; i < 8 ; i++)
    {
        addrPtr->ipMtRegs.qoSProfile2MTTCQueuesMapTable[i] = 0x0C800100 + 4 * i;
    }
    addrPtr->ipMtRegs.mllTableBase = 0x0C880000;
    for (i = 0 ; i < 2 ; i++)
    {
        offset = i * 0x100;
        addrPtr->ipMtRegs.mllOutInterfaceCfg[i]    = 0x0C800980 + offset;
        addrPtr->ipMtRegs.mllOutMcPktCnt[i]        = 0x0C800900 + offset;
    }
}

/*******************************************************************************
* cheetah2PclRegsInit
*
* DESCRIPTION:
*       This function initializes the Internal InLif Tables registers
*       struct for devices that belong to REGS_FAMILY_1 registers set. -- cheetah 2
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
static void cheetah2PclRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    addrPtr->pclRegs.tcamReadMaskBase            = 0x0B880008;
    addrPtr->pclRegs.tcamReadPatternBase         = 0x0B880000;
    addrPtr->pclRegs.tcamReadMaskControlBase     = 0x0B880000;
    addrPtr->pclRegs.tcamReadPatternControlBase  = 0x0B88000C;

    addrPtr->pclRegs.epclGlobalConfig        = 0x07C00000;
    /* bit per port registers */
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclCfgTblAccess,0x07C00004);
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclEnPktNonTs,0x07C00008);
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclEnPktTs,0x07C0000C);
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclEnPktToCpu,0x07C00010);
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclEnPktFromCpuData,0x07C00014);
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclEnPktFromCpuControl,0x07C00018);
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclEnPktToAnalyzer,0x07C0001C);
    /* TCP/UDP port comparators (8 register sequences) */
    addrPtr->pclRegs.ipclTcpPortComparator0  = 0x0B800600;
    addrPtr->pclRegs.ipclUdpPortComparator0  = 0x0B800640;
    addrPtr->pclRegs.epclTcpPortComparator0  = 0x07C00100;
    addrPtr->pclRegs.epclUdpPortComparator0  = 0x07C00140;
}

/*******************************************************************************
* cheetah3GlobalRegsInit
*
* DESCRIPTION:
*       This function initializes the global registers struct for devices that
*       belong to REGS_FAMILY_1 registers set.-- cheetah 3
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with global regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetah3GlobalRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    addrPtr->globalRegs.lastReadTimeStampReg = 0x00000040;
}

/*******************************************************************************
* prvCpssDxCh3HwRegAddrInit
*
* DESCRIPTION:
*       This function initializes the registers struct for cheetah-3 devices.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxCh3HwRegAddrInit
(
    IN GT_U32 devNum
)
{
    GT_STATUS rc;
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC *regsAddrPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);


    /* first set the "Cheetah" part */
    rc = prvCpssDxCh2HwRegAddrInit(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* set the specific "cheetah 3" parts */
    regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    cheetah3BridgeRegsInit(regsAddrPtr);
    cheetah3EgrTxRegsInit(devNum,regsAddrPtr);
    cheetah3CncRegsInit(regsAddrPtr);
    cheetah3PclRegsInit(devNum, regsAddrPtr);
    cheetah3PolicerRegsInit(regsAddrPtr);
    cheetah3MacRegsInit(devNum,regsAddrPtr);
    if (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {
        cheetah3IpRegsInit(regsAddrPtr);
    }
    cheetah3GlobalRegsInit(regsAddrPtr);

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxCh2HwRegAddrInit
*
* DESCRIPTION:
*       This function initializes the registers struct for cheetah2 devices.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxCh2HwRegAddrInit
(
    IN GT_U32 devNum
)
{
    GT_STATUS rc;
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC *regsAddrPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);


    /* first set the "Cheetah" part */
    rc = prvCpssDxChHwRegAddrInit(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* set the specific "cheetah 2" parts */
    regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    cheetah2BridgeRegsInit(regsAddrPtr);
    cheetah2GlobalRegsInit(regsAddrPtr);
    cheetah2BufMngRegsInit(regsAddrPtr);
    cheetah2EgrTxRegsInit(devNum,regsAddrPtr);
    cheetah2MacRegsInit(devNum,regsAddrPtr);
    if (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {
        cheetah2Ipv4RegsInit(regsAddrPtr);
        cheetah2Ipv6RegsInit(regsAddrPtr);
        cheetah2IpRegsInit(regsAddrPtr);
        cheetah2IpMtRegsInit(regsAddrPtr);
    }
    cheetah2PclRegsInit(regsAddrPtr);
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxCh2HwRegAddrRemove
*
* DESCRIPTION:
*       This function release the memory that was allocated by the function
*       prvCpssDxCh2HwRegAddrInit(...)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum  - The PP's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxCh2HwRegAddrRemove
(
    IN  GT_U32 devNum
)
{
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);


    /* first free the "Cheetah" part */
    rc = prvCpssDxChHwRegAddrRemove(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* free the specific "cheetah 2" parts */

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxCh3HwRegAddrRemove
*
* DESCRIPTION:
*       This function release the memory that was allocated by the function
*       prvCpssDxCh3HwRegAddrInit(...)
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum  - The PP's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxCh3HwRegAddrRemove
(
    IN  GT_U32 devNum
)
{
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);


    /* first free the "Cheetah2" part */
    rc = prvCpssDxCh2HwRegAddrRemove(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* free the specific "cheetah 3" parts */

    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChHwRegAddrStcInfoGet
*
* DESCRIPTION:
*   This function return the address of the registers struct of cheetah devices.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       regAddrPtrPtr - (pointer to) address of the registers struct
*       sizePtr - (pointer to) size of registers struct
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwRegAddrStcInfoGet
(
    IN  GT_U8     devNum,
    OUT GT_U32  **regAddrPtrPtr,
    OUT GT_U32   *sizePtr
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    *regAddrPtrPtr = (GT_U32 *)PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);
    *sizePtr = sizeof(*(PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)));

    return GT_OK;
}

/*******************************************************************************
* xCatIpclTccUnitRegsInit
*
* DESCRIPTION:
*       This function initializes the PCL registers struct for devices that
*       belong to REGS_FAMILY_1 registers set. -- xCat
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       addrPtr - pointer to registers struct
*
* OUTPUTS:
*       addrPtr - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCatIpclTccUnitRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32  ii;

    for (ii = 0; (ii < 12); ii++)
    {
        addrPtr->pclRegs.tcamWriteData[ii] = 0x0d000100 + (4 * ii); /*in ch3 was 0x0B800100*/
    }

    addrPtr->pclRegs.tcamOperationTrigger = 0x0d000138/*in ch3 was 0x0B800138*/;
    addrPtr->pclRegs.tcamOperationParam1  = 0x0d000130/*in ch3 was 0x0B800130*/;
    addrPtr->pclRegs.tcamOperationParam2  = 0x0d000134/*in ch3 was 0x0B800134*/;
    addrPtr->pclRegs.tcamReadPatternBase  = 0x0d040000/*in ch3 was 0x0BA00000*/;
    addrPtr->pclRegs.actionTableBase      = 0x0d0b0000/*in ch3 was 0x0B880000*/;
    addrPtr->pclRegs.pclTcamConfig0       = 0x0d000208/*in ch3 was 0x0B820108*/;
    addrPtr->pclRegs.pclTcamConfig2       = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.pclTcamControl       = PRV_CPSS_SW_PTR_ENTRY_UNUSED ;

    /* CNC related registers */
    addrPtr->pclRegs.cncCountMode                  = 0x0B800088;
    addrPtr->pclRegs.cncl2l3IngressVlanCountEnable = 0x0B800080;
    addrPtr->pclRegs.actionTableBase               = 0x0e0b0000;
    addrPtr->pclRegs.tcamOperationTrigger = 0x0E000138/*in ch3 was 0x0B800138*/;
    addrPtr->pclRegs.tcamOperationParam1  = 0x0E000130/*in ch3 was 0x0B800130*/;
    addrPtr->pclRegs.tcamOperationParam2  = 0x0E000134/*in ch3 was 0x0B800134*/;
    addrPtr->pclRegs.tcamReadPatternBase  = 0x0E040000/*in ch3 was 0x0BA00000*/;
    addrPtr->pclRegs.pclTcamConfig0       = 0x0E000208/*in ch3 was 0x0B820108*/;
    for (ii = 0; (ii < 12); ii++)
    {
        addrPtr->pclRegs.tcamWriteData[ii] = 0x0E000100 + (4 * ii); /*in ch3 was 0x0B800100*/
    }

}

/*******************************************************************************
* xCatIpvxTccUnitRegsInit
*
* DESCRIPTION:
*       This function initializes the IPvX registers struct for devices that
*       belong to REGS_FAMILY_1 registers set. -- xCat
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCatIpvxTccUnitRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32      i;  /* loop iterator */

    addrPtr->ipRegs.routerTtTcamAccessCtrlReg      = 0x0e80041c;/*in ch3 was  0x0280041C*/
    addrPtr->ipRegs.routerTtTcamAccessDataCtrlReg  = 0x0e800418;/*in ch3 was  0x02800418*/
    addrPtr->ipRegs.routerTtTcamAccessDataRegsBase = 0x0e800400;/*in ch3 was  0x02800400*/

    /* only base of all TCAM will be initialized */
    addrPtr->ipRegs.routerTtTcamBankAddr[0] = 0x0e840000;

    addrPtr->ipRegs.ipTcamConfig0       = 0x0e80096c;/*in ch3 was  0x0280096C*/
    addrPtr->ipRegs.ipTcamControl = PRV_CPSS_SW_PTR_ENTRY_UNUSED;

    /* The following registers are not used in xCat */
    /* Tcam bank addresses, starting from the second one */
    for (i = 1; i <= 4; i++)
    {
        addrPtr->ipRegs.routerTtTcamBankAddr[i] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    }
}

/*******************************************************************************
* xCatMacRegsInit
*
* DESCRIPTION:
*       This function initializes MAC,SERDESs related registers struct
*       for devices that belong to REGS_FAMILY_1 registers set. -- xCat
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCatMacRegsInit
(
    IN GT_U32          devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U8   i;
    GT_U32  lane;/*(serdes) lanes*/
    GT_U32  offset, offset1;
    GT_U32  *u32Ptr;
    GT_U32  size;

    /* Init MAC registers   */
    for (i = 0; (i < PRV_CPSS_PP_MAC(devNum)->numOfPorts); i++)
    {
        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            /* set the the non exists ports of lion with regAdder 'unused' */
            if((i & 0xf) > 0xb)
            {
                u32Ptr = (GT_U32*)(&addrPtr->macRegs.perPortRegs[i]);
                size = (sizeof(struct _perPortRegs) / sizeof(GT_U32));
                defaultAddressUnusedSet(u32Ptr,size);

                continue;
            }
        }

        offset = i * 0x400;

        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            offset = (i & 0xf) * 0x400;
        }

        /* init all gig specific addresses for all ports. */
        /* serialParameters - per port */
        addrPtr->macRegs.perPortRegs[i].serialParameters = 0x0A800014 + offset;

        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
            addrPtr->macRegs.perPortRegs[i].serialParameters1 = 0x0A800094 + offset;

        addrPtr->macRegs.perPortRegs[i].autoNegCtrl      = 0x0A80000C + offset;
        addrPtr->macRegs.perPortRegs[i].serdesCnfg       = 0x0A800028 + offset;

        /* PRBS registers */
        addrPtr->macRegs.perPortRegs[i].prbsCheckStatus  = 0x0A800038 + offset;
        addrPtr->macRegs.perPortRegs[i].prbsErrorCounter = 0x0A80003C + offset;

        addrPtr->macRegs.perPortRegs[i].ccfcFcTimerBaseGig = 0x0A800058 + offset;

        addrPtr->macRegs.perPortRegs[i].Config100Fx = 0x0A80002C + offset;

        if (((PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E)
            && (i >= 24))
            || (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E))
        {
            addrPtr->macRegs.perPortRegs[i].ccfcFcTimerBaseXg = 0x08800038 + offset;
            addrPtr->macRegs.perPortRegs[i].xgGlobalConfReg0 = 0x08800200 + offset;

            if(PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[i].portType < PRV_CPSS_PORT_XG_E)
            {
                /* Init XG registers for all types of ports >=24 to be able to change interface on the fly */
                for(lane = 0; lane < 4; lane++)
                {
                    offset1 = i * 0x0400 + lane * 0x044;
                    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
                    {
                        offset1 = (i & 0xf) * 0x0400 + lane * 0x044;
                    }

                    addrPtr->macRegs.perPortRegs[i].laneConfig0[lane] =
                         0x08800250 + offset1;
                    addrPtr->macRegs.perPortRegs[i].laneConfig1[lane] =
                         0x08800254 + offset1;
                    addrPtr->macRegs.perPortRegs[i].disparityErrorCounter[lane] =
                         0x0880026C + offset;
                    addrPtr->macRegs.perPortRegs[i].prbsErrorCounterLane[lane] =
                         0x08800270 + offset1;
                    addrPtr->macRegs.perPortRegs[i].laneStatus[lane] =
                         0x0880025c + offset1;
                    addrPtr->macRegs.perPortRegs[i].cyclicData[lane] =
                         0x08800284 + offset1;
                }
            }
        }

        if(PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[i].portType < PRV_CPSS_PORT_XG_E)
        {
            if (((PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E)
                && (i >= 24))
                || (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E))
            {
                /* override the settings of MIB MAC counters , because ports
                    24..27 share counters with the 'XG' ports */
                addrPtr->macRegs.perPortRegs[i].macCounters        = 0x09000000 + (i * 0x20000);
                addrPtr->macRegs.perPortRegs[i].macCaptureCounters = 0x09000080 + (i * 0x20000);

                addrPtr->macRegs.perPortRegs[i].xgMibCountersCtrl = 0x08800030 + (i * 0x400);

                /* save addresses of XG mac for XG capable ports */
                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl =
                    0x08800000 + offset;
                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl1 =
                    0x08800004 + offset;
                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl2 =
                    0x08800008 + offset;
                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl3 =
                    0x0880001C + offset;
            }

            if ((CPSS_PP_FAMILY_DXCH_XCAT2_E == PRV_CPSS_PP_MAC(devNum)->devFamily )
                && (i >= 24))
            {/* clear Tri-speed MAC init for XG ports of xCat2 */
                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl =
                    PRV_CPSS_SW_PTR_ENTRY_UNUSED;

                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl1 =
                    PRV_CPSS_SW_PTR_ENTRY_UNUSED;

                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl2 =
                    PRV_CPSS_SW_PTR_ENTRY_UNUSED;

                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl3 =
                    PRV_CPSS_SW_PTR_ENTRY_UNUSED;
            }

            /* Init GE Mac control registers for all ports */
            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_FE_E].macCtrl =
                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl =
                    0x0A800000 + offset;

            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_FE_E].macCtrl1 =
                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl1 =
                    0x0A800004 + offset;

            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_FE_E].macCtrl2 =
                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl2 =
                    0x0A800008 + offset;

            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_FE_E].macCtrl3 =
                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl3 =
                    0x0A800048 + offset;

            /* Mac status registers     */
            addrPtr->macRegs.perPortRegs[i].macStatus = 0x0A800010 + offset;
        }
        else
        {
            /* no need to override ch3 settings of the XG port */

            if((PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,i) == 10)
             && (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E))
            {/* init XLG addresses */
                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XLG_E].macCtrl =
                    0x08803000;

                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XLG_E].macCtrl1 =
                    0x08803004;

                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XLG_E].macCtrl2 =
                    0x08803008;

                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XLG_E].macCtrl3 =
                    0x0880301C;

                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XLG_E].miscConfig =
                    0x08803060;
            }
        }

        if ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
            && (i >= 24))
        {
            addrPtr->macRegs.perPortRegs[i].macCounters        = 0x09000000 + (i * 0x20000);
            addrPtr->macRegs.perPortRegs[i].macCaptureCounters = 0x09000080 + (i * 0x20000);
            addrPtr->macRegs.perPortRegs[i].xgMibCountersCtrl  = 0x0A800044 + (i * 0x400);
        }
    }



    if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_LION_E)
    {
        for(i=0 ; i < 6 ; i++)
        {
            addrPtr->macRegs.QSGMIIControlAndStatus[i].QSGMIIControlRegister0 = 0x0a80004c + 4*i*0x400;
            addrPtr->macRegs.QSGMIIControlAndStatus[i].QSGMIIStatusRegister   = 0x0a800050 + 4*i*0x400;
            addrPtr->macRegs.QSGMIIControlAndStatus[i].QSGMIIPRBSErrorCounter = 0x0a800054 + 4*i*0x400;
            addrPtr->macRegs.QSGMIIControlAndStatus[i].QSGMIIControlRegister1 = 0x0a80044c + 4*i*0x400;
            addrPtr->macRegs.QSGMIIControlAndStatus[i].QSGMIIControlRegister2 = 0x0a80084c + 4*i*0x400;
        }
    }

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {
        PORTS_BMP_REG_SET_MAC(
            addrPtr->haRegs.cscdHeadrInsrtConf,0x0F000004);

        PORTS_BMP_REG_SET_MAC(
            addrPtr->haRegs.eggrDSATagTypeConf,0x0F000020);
    }
    else
    {
        PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
            addrPtr->haRegs.cscdHeadrInsrtConf,0x0F000004);

        PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
            addrPtr->haRegs.eggrDSATagTypeConf,0x0F000020);
    }

    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->haRegs.invalidCrcModeConfigReg,0x0F000400);
}

/*******************************************************************************
* xCatIpMtRegsInit
*
* DESCRIPTION:
*       This function initializes the IP Multi Traffic registers struct for
*       devices that belong to REGS_FAMILY_1 registers set . -- -- xCat
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with IPv4 regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCatIpMtRegsInit
(
    IN GT_U32          devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32 i;
    GT_U32 offset;

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {
        return;
    }

    addrPtr->ipMtRegs.mllGlobalReg = 0x0d800000;
    addrPtr->ipMtRegs.multiTargetRateShapingReg = 0x0d800210;
    addrPtr->ipMtRegs.mtUcSchedulerModeReg = 0x0d800214;
    addrPtr->ipMtRegs.mtTcQueuesSpEnableReg = 0x0d800200;
    addrPtr->ipMtRegs.mtTcQueuesWeightReg = 0x0d800204;
    addrPtr->ipMtRegs.mtTcQueuesGlobalReg = 0x0d800004;
    addrPtr->ipMtRegs.mllMcFifoFullDropCnt = 0x0d800984;
    for (i = 0 ; i < 8 ; i++)
    {
        addrPtr->ipMtRegs.qoSProfile2MTTCQueuesMapTable[i] = 0x0d800100 + 4 * i;
    }
    addrPtr->ipMtRegs.mllTableBase = 0x0d880000;
    for (i = 0 ; i < 2 ; i++)
    {
        offset = i * 0x100;
        addrPtr->ipMtRegs.mllOutInterfaceCfg[i]    = 0x0d800980 + offset;
        addrPtr->ipMtRegs.mllOutMcPktCnt[i]        = 0x0d800900 + offset;
    }

    if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_LION_E)
    {
        addrPtr->ipMtRegs.dft1Reg = 0x0D800014;
        addrPtr->ipMtRegs.mllMetalFix = 0x0D80000C;
    }
}

/*******************************************************************************
* xCatIpRegsInit
*
* DESCRIPTION:
*       This function initializes the IP registers struct for
*       devices that belong to REGS_FAMILY_1 registers set. -- xCat
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       addrPtr - pointer to registers struct
*
* OUTPUTS:
*       addrPtr - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCatIpRegsInit
(
   INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32      i;  /* loop iterator */

    addrPtr->haRegs.hdrAltGlobalConfig = 0x0F000100;
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->haRegs.routerHdrAltEnMacSaModifyReg,0x0F000104);
    addrPtr->haRegs.routerHdrAltMacSaModifyMode[0] = 0x0F000120;
    addrPtr->haRegs.routerHdrAltMacSaModifyMode[1] = 0x0F000124;
    addrPtr->haRegs.routerMacSaBaseReg[0] = 0x0F000108;
    addrPtr->haRegs.routerMacSaBaseReg[1] = 0x0F00010C;
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->haRegs.routerDevIdModifyEnReg,0x0F000110);


    /* The following registers are unused for xCat */
    /* Router TCAM Test */
    for (i = 0; i < 20; i++)
    {
        addrPtr->ipRegs.routerTcamTest.pointerRelReg[i] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    }

    addrPtr->ipRegs.routerTcamTest.configStatusReg = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->ipRegs.routerTcamTest.opcodeCommandReg = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->ipRegs.routerTcamTest.opcodeExpectedHitReg = PRV_CPSS_SW_PTR_ENTRY_UNUSED;

    addrPtr->ipRegs.ucRoutingEngineConfigurationReg = 0x02800E3C;

    addrPtr->ipRegs.routerPerPortSipSaEnable0 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
}

/*******************************************************************************
* lionIpRegsInit
*
* DESCRIPTION:
*       This function initializes the IP registers struct for
*       devices that belong to REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr - pointer to registers struct
*
* OUTPUTS:
*       addrPtr - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lionIpRegsInit
(
   IN GT_U32          devNum,
   INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32 offset, i;

    devNum = devNum;

    addrPtr->ipRegs.ctrlReg0 = 0x02800E3C;

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        addrPtr->ipRegs.routerPerPortSipSaEnable0 = 0x02800014;
    }


    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        /* Router TCAM Test */
        for (i = 0; i < 20; i++)
        {
            offset = i * 0x4;
            addrPtr->ipRegs.routerTcamTest.pointerRelReg[i] = 0x0D800100 + offset;
        }

        addrPtr->ipRegs.routerTcamTest.configStatusReg = 0x0D800150;
        addrPtr->ipRegs.routerTcamTest.opcodeCommandReg = 0x0D800154;
        addrPtr->ipRegs.routerTcamTest.opcodeExpectedHitReg = 0x0D800158;
    }

    return;
}

/*******************************************************************************
* xCatEgrTxRegsInit
*
* DESCRIPTION:
*       This function initializes the Egress and Transmit (Tx) Queue
*       Configuration registers - L2 and L3 Port Isolation table entries.
*
* INPUTS:
*       devNum     - The PP's device number.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCatEgrTxRegsInit
(
    IN GT_U32    devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return;
    }

    addrPtr->egrTxQConf.l2PortIsolationTableBase = 0x01E40004;
    addrPtr->egrTxQConf.l3PortIsolationTableBase = 0x01E40008;
}

/*******************************************************************************
* xCatBrgRegsInit
*
* DESCRIPTION:
*       This function initializes the Brigde registers struct for
*       devices that belong to REGS_FAMILY_1 registers set. -- xCat
*
* INPUTS:
*       devNum  - device number
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCatBrgRegsInit
(
    IN GT_U32                               devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32 ii; /* loop iterator */


    for (ii = 0; ii < 4; ii++)
    {
        addrPtr->bridgeRegs.bridgeIngressVlanEtherTypeTable[ii]= 0x0C000300 + ii * 4;
        addrPtr->haRegs.bridgeEgressVlanEtherTypeTable[ii] = 0x0F000430 + ii * 4;
        addrPtr->haRegs.bridgeEgressPortTag0TpidSelect[ii] = 0x0F000440 + ii * 4;
        addrPtr->haRegs.bridgeEgressPortTag1TpidSelect[ii] = 0x0F000460 + ii * 4;
    }

    for (ii = 4; ii < (PORTS_NUM_CNS/8); ii++)
    {
        addrPtr->haRegs.bridgeEgressPortTag0TpidSelect[ii] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->haRegs.bridgeEgressPortTag1TpidSelect[ii] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    }

    for (ii = 0; ii < 32; ii++)
    {
        if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_LION_E)
        {
            addrPtr->bridgeRegs.bridgeIngressVlanSelect[ii] = 0x0C000310 + ii * 4;
        }
    }

	#ifndef __AX_PLATFORM_XCAT__
	/* For interface VLAN */
	addrPtr->bridgeRegs.macTblAction0 = 0x06000004;
    addrPtr->bridgeRegs.macTblAction1 = 0x06000008;
    addrPtr->bridgeRegs.macTblAction2 = 0x06000020;
    addrPtr->bridgeRegs.macControlReg  = 0x06000000;   /* bit 19 AA&TA msg to CPU */

    /* init all arrays of per port registers */
    for (ii = 0; ii  < PRV_CPSS_PP_MAC(devNum)->numOfPorts; ii++)
    {
        /* bridge configuration registers 0..1 */
        addrPtr->bridgeRegs.portControl[ii] = 0x02000000 + ii * 0x1000;

        addrPtr->bridgeRegs.vlanRegs.portPrvVlan[ii] = 0x02000010 + ii * 0x1000;

        /* both "port based" and "port and protocol based" */
        /* VLAN and QoS cofiguration tables */
        /* have only indirect and "control + data" access */
        /* not used portVid and protoVidBased*/

        /* brdByteLimit - is in bridge configuration register1 */
        /* use portPrvVlan instead of brdByteLimit */

        /* brdByteWindow - in global register 0x02040140 */
    }
	
    addrPtr->bridgeRegs.cpuPortControl = 0x0203F000;

    addrPtr->bridgeRegs.deviceTable = 0x06000068;    /* cpssDxChBrgFdbDeviceTableSet() */


	/* Here we add, because the nam will R/W those following regs  zhangdi */
    /* two VLAN ranges */
    addrPtr->bridgeRegs.vlan0 = 0x020400A8;
    /* two protocol IDs 0x8100 in 275 */
    addrPtr->bridgeRegs.vlan1 = 0x72410;
	/*Table 402: VLAN EtherType (EVLANE) Register */
    /*Offset: Port0: 0x72410 Port1: 0x76410  ??????? */ 
    /*The BobCat implements two Gigabit Ethernet (GbE) controllers that support Port0 RGMII, port1 
     *RGMIIA
     */

    addrPtr->bridgeRegs.bridgeGlobalConfigRegArray[0] = 0x02040000;  /* cpssDxChBrgGenRipV1MirrorToCpuEnable */
    addrPtr->bridgeRegs.bridgeGlobalConfigRegArray[1] = 0x02040004;
    addrPtr->bridgeRegs.bridgeGlobalConfigRegArray[2] = 0x0204000C;

    /*
        Classified Distribution Table 379:
        IEEE Reserved Multicast Configuration Register<n> (0<=n<16)
        Offset:  Reg0: 0x02000800, Reg1:0x02001800... Reg15:0x0200F800
        (16 Registers in step of 0x1000)
    */
    addrPtr->bridgeRegs.macRangeFltrBase[0] = 0x02000800;	/* cpssDxChBrgGenIeeeReservedMcastProtCmdSet */
	
	/* Add end */
    #endif
	
    /* Protocols Encapsulation Configuration */
    addrPtr->bridgeRegs.vlanRegs.protoEncBased = 0x0C000098;
    addrPtr->bridgeRegs.vlanRegs.protoEncBased1 = 0x0C00009C ;

    /* 6 registers 0x0C000080-0x0C000094 for 12 protocols */
    addrPtr->bridgeRegs.vlanRegs.protoTypeBased = 0x0C000080;

    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->haRegs.haVlanTransEnReg,0x0F000130);

    /* QoS Profile registers */
    addrPtr->haRegs.qosProfile2ExpTable     = 0x0F000300;
    addrPtr->bridgeRegs.qosProfileRegs.dscp2QosProfileMapTable = 0x0c000400;
    addrPtr->haRegs.qosProfile2DpTable      = 0x0F000340;
    /* CFI UP -> Qos */
    addrPtr->bridgeRegs.qosProfileRegs.up2QosProfileMapTable   = 0x0c000440;
    addrPtr->bridgeRegs.qosProfileRegs.exp2QosProfileMapTable  = 0x0c000460;
    addrPtr->bridgeRegs.qosProfileRegs.dscp2DscpMapTable       = 0x0C000900;

    /* TO ANALYZER VLAN Adding Configuration Registers */
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->haRegs.toAnalyzerVlanAddConfig,0x0F000404);

    /* Ingress Analyzer VLAN Tag Configuration Register */
    addrPtr->haRegs.ingAnalyzerVlanTagConfig = 0x0F000408;

    /* Egress Analyzer VLAN Tag Configuration Register */
    addrPtr->haRegs.egrAnalyzerVlanTagConfig = 0x0F00040C;

    /* IPv6 Solicited-Node Multicast Address Configuration/Mask Register0 */
    addrPtr->bridgeRegs.ipv6McastSolicitNodeAddrBase = 0x0C000040;
    addrPtr->bridgeRegs.ipv6McastSolicitNodeMaskBase = 0x0C000050;

    addrPtr->bridgeRegs.macRangeFltrBase[1] = 0x02000808;
    addrPtr->bridgeRegs.macRangeFltrBase[2] = 0x02000810;
    addrPtr->bridgeRegs.macRangeFltrBase[3] = 0x02000818;

    addrPtr->bridgeRegs.ieeeReservedMcastProfileSelect0 = 0x0200080C;
    addrPtr->bridgeRegs.ieeeReservedMcastProfileSelect1 = 0x0200081C;
}

/*******************************************************************************
* xCatTtiRegsInit
*
* DESCRIPTION:
*       This function initializes the TTI registers struct for
*       devices that belong to REGS_FAMILY_1 registers set. -- xCat
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCatTtiRegsInit
(
   INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    addrPtr->ttiRegs.pclIdConfig0        = 0x0C000010;
    addrPtr->ttiRegs.pclIdConfig1        = 0x0C000014;
    addrPtr->ttiRegs.ttiIpv4GreEthertype = 0x0C000018;
    addrPtr->ttiRegs.ccfcEthertype       = 0x0C000024;
    addrPtr->ttiRegs.specialEthertypes   = 0x0C000028;
    addrPtr->ttiRegs.mplsEthertypes      = 0x0C000030;
    addrPtr->ttiRegs.trunkHashCfg[0]     = 0x0C000070;
    addrPtr->ttiRegs.trunkHashCfg[1]     = 0x0C000074;
    addrPtr->ttiRegs.trunkHashCfg[2]     = 0x0C000078;
}

/*******************************************************************************
* xCatHaRegsInit
*
* DESCRIPTION:
*       This function initializes the HA registers struct for
*       devices that belong to REGS_FAMILY_1 registers set. -- xCat
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr - pointer to registers struct
*
* OUTPUTS:
*       addrPtr - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCatHaRegsInit
(
   IN GT_U32 devNum,
   INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    addrPtr->haRegs.hdrAltCnmHeaderConfig  = 0x0F000424;
    addrPtr->haRegs.hdrAltMplsEthertypes   = 0x0F000480;
    addrPtr->haRegs.hdrAltIEthertype       = 0x0F000484;

    if (1 == PRV_CPSS_DXCH_XCAT_A3_ONLY_CHECK_MAC(devNum))
    {
        addrPtr->haRegs.hdrAltCpidReg0     = 0x0F000428;
        addrPtr->haRegs.hdrAltCpidReg1     = 0x0F00042C;
    }

}
/*******************************************************************************
* xCatBridgeRegsInit
*
* DESCRIPTION:
*       This function initializes the registers bridge struct for devices that
*       belong to REGS_FAMILY_1 registers set. -- xCat
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with bridge regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCatBridgeRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC *addrPtr
)
{
    addrPtr->bridgeRegs.vntReg.vntGreEtherTypeConfReg = 0x0C000028;
    addrPtr->bridgeRegs.vntReg.vntCfmEtherTypeConfReg = 0x0C000028;
}

/*******************************************************************************
* xCatGlobalRegsInit
*
* DESCRIPTION:
*       This function initializes the Global registers struct for
*       devices that belong to REGS_FAMILY_1 registers set. -- xCat
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCatGlobalRegsInit
(
   INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    addrPtr->globalRegs.extendedGlobalControl  = 0x0000005C;
    addrPtr->globalRegs.extendedGlobalControl2 = 0x0000008C;
    addrPtr->globalRegs.ftdllReg               = 0x0000000C;
    addrPtr->globalRegs.analogCfgReg           = 0x0000009C;

    #ifndef __AX_PLATFORM_XCAT__    /* add compare to 275, zhangdi 2011-03-17 */
    GT_U8   i;
    for (i = 0; i < 4; i++)
    {
        addrPtr->globalRegs.addrUpdate[i] = 0x06000040 + i * 4;
    }
    addrPtr->globalRegs.addrUpdateControlReg = 0x06000050;

    addrPtr->globalRegs.globalControl          = 0x00000058;

    #if 0 
    addrPtr->globalRegs.extendedGlobalControl  = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->globalRegs.extendedGlobalControl2 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->globalRegs.sampledAtResetReg      = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    #endif
    addrPtr->globalRegs.addrCompletion         = 0x00000000;
	
    addrPtr->globalRegs.auQBaseAddr            = 0x000000C0;
    addrPtr->globalRegs.auQControl             = 0x000000C4;
    addrPtr->globalRegs.fuQBaseAddr            = 0x000000C8;
    addrPtr->globalRegs.fuQControl             = 0x000000CC;
	
    addrPtr->globalRegs.interruptCoalescing    = 0x000000E0;
    addrPtr->globalRegs.dummyReadAfterWriteReg = 0x000000F0;

    addrPtr->globalRegs.cpuPortCtrlReg         = 0x000000A0;

	#endif
	
}

/*******************************************************************************
* xCatMirrorRegsInit
*
* DESCRIPTION:
*       This function initializes the Global registers struct for
*       devices that belong to REGS_FAMILY_1 registers set. -- xCat
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCatMirrorRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U8   i;

    addrPtr->bufferMng.eqBlkCfgRegs.sniffPortsCfg = 0x0B00B020;

    for (i = 0; i < 7; i++)
    {
        /* Mirror Interface Parameter Registers */
        addrPtr->bufferMng.eqBlkCfgRegs.mirrorInterfaceParameterReg[i] =
            0x0B00B020 + i * 4;
    }

    for (i = 0; i < 3; i++)
    {
        addrPtr->bufferMng.eqBlkCfgRegs.portRxMirrorIndex[i] = 0x0B00B000 + i * 4;
        addrPtr->bufferMng.eqBlkCfgRegs.portTxMirrorIndex[i] = 0x0B00B010 + i * 4;
    }

    addrPtr->bufferMng.eqBlkCfgRegs.analyzerPortGlobalConfig = 0x0B00B040;
}

/*******************************************************************************
* xCatBufMngRegsInit
*
* DESCRIPTION:
*       This function initializes the buffer management registers struct
*       for devices that belong to REGS_FAMILY_1 registers set.-- xCat
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with buffer management regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCatBufMngRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U8   i;

    for (i = 0; i < 7; i++)
    {
        /* Port<4n,4n+1,4n+2,4n+3> TrunkNum Configuration Register<n> (0<=n<7) */
        /* num of registers : (corePpDevs[devNum]->numOfPorts / 4) + 1 */
        /* use CHEETAH_TRUNK_ID_CONIFG_TABLE_OFFSET_CNS for offset */
        addrPtr->haRegs.trunkNumConfigReg[i] = 0x0F00002C + 4*i;
    }
}

/*******************************************************************************
* lionBufMngRegsInit
*
* DESCRIPTION:
*       This function initializes the buffer management registers struct
*       for devices that belong to REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with buffer management regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lionBufMngRegsInit
(
    IN GT_U32                               devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U8   i;

    addrPtr->bufferMng.eqBlkCfgRegs.cscdEgressMonitoringEnableConfReg = 0x0B000074;

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        for (i = 0; i < 2; i++)
        {
            addrPtr->bufferMng.txdmaThresholdOverrideEn[i] = 0x0F800040 + i*4;
        }

        addrPtr->bufferMng.txdmaBufMemXgFifosThreshold = 0x0F800018;
    }
}

/*******************************************************************************
* xCatBcnRegsInit
*
* DESCRIPTION:
*       This function initializes the BCN registers struct
*       for devices that belong to REGS_FAMILY_1 registers set.-- xCat
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with buffer management regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCatBcnRegsInit
(
    IN GT_U32                               devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    if ((PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT_E)
        && (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
    {
        /* those are 'XCAT only' registers */
        addrPtr->bcnRegs.bcnControlReg =               PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->bcnRegs.portsBcnAwarenessTbl =        PRV_CPSS_SW_PTR_ENTRY_UNUSED;

        addrPtr->bcnRegs.bcnPauseTriggerEnableReg =    PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->bcnRegs.bcnGlobalPauseThresholdsReg = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->bcnRegs.portsBcnProfileReg0 =         PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->bcnRegs.portsSpeedIndexesReg0 =       PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    }
    else
    {
        addrPtr->bcnRegs.bcnControlReg =               0x0A400000;/*xcat only*/
        addrPtr->bcnRegs.portsBcnAwarenessTbl =        0x0A400030;/*xcat only*/
        addrPtr->bcnRegs.bcnPauseTriggerEnableReg =    0x0A40000C;/*xcat only*/
        addrPtr->bcnRegs.bcnGlobalPauseThresholdsReg = 0x0A400014;/*xcat only*/
        addrPtr->bcnRegs.portsBcnProfileReg0 =         0x0A400020;/*xcat only*/
        addrPtr->bcnRegs.portsSpeedIndexesReg0 =       0x0A400034;/*xcat only*/
    }

}

/*******************************************************************************
* xCatSerdesRegsInit
*
* DESCRIPTION:
*       This function initializes the SERDES registers.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with bridge regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCatSerdesRegsInit
(
    IN GT_U32 devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC *addrPtr
)
{
    GT_U32 i, offset;
    GT_U32 devType;     /* device type          */
    GT_U32 startSerdesIndx;
    GT_U32 maxSerdesIndx; /* amount of SerDes */

    /* XCAT - default */
    maxSerdesIndx = PRV_CPSS_XCAT_SERDES_NUM_CNS;
    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {
        /* XCAT2 - override */
        maxSerdesIndx = PRV_CPSS_XCAT2_SERDES_NUM_CNS;;
    }

    /* define type of xCat device either Gig or Fast */
    devType = PRV_CPSS_PP_MAC(devNum)->devType;
    switch(devType)
    {
        case PRV_CPSS_DXCH_XCAT_FE_DEVICES_CASES_MAC:
        case PRV_CPSS_DXCH_XCAT2_FE_DEVICES_CASES_MAC:
            startSerdesIndx = 6;
            break;
        default:
            startSerdesIndx = 0;
            break;
    }

    for (i = startSerdesIndx; (i < maxSerdesIndx); i++)
    {
        offset = ((i / 2) * 0x1000) + ((i % 2) * 0x400);
        addrPtr->serdesConfig[i].calibrationReg0 = 0x09800224 + offset;
        addrPtr->serdesConfig[i].calibrationReg1 = 0x0980022C + offset;
        addrPtr->serdesConfig[i].calibrationReg2 = 0x09800230 + offset;
        addrPtr->serdesConfig[i].calibrationReg3 = 0x09800234 + offset;
        addrPtr->serdesConfig[i].calibrationReg5 = 0x0980023C + offset;
        addrPtr->serdesConfig[i].calibrationReg7 = 0x09800228 + offset;
        addrPtr->serdesConfig[i].pllIntpReg1     = 0x0980020C + offset;
        addrPtr->serdesConfig[i].pllIntpReg2     = 0x09800210 + offset;
        addrPtr->serdesConfig[i].pllIntpReg3     = 0x09800214 + offset;
        addrPtr->serdesConfig[i].pllIntpReg4     = 0x09800218 + offset;
        addrPtr->serdesConfig[i].pllIntpReg5     = 0x0980021C + offset;
        addrPtr->serdesConfig[i].transmitterReg0 = 0x09800250 + offset;
        addrPtr->serdesConfig[i].transmitterReg1 = 0x09800254 + offset;
        addrPtr->serdesConfig[i].transmitterReg2 = 0x09800258 + offset;
        addrPtr->serdesConfig[i].receiverReg1    = 0x0980027C + offset;
        addrPtr->serdesConfig[i].receiverReg2    = 0x09800280 + offset;
        addrPtr->serdesConfig[i].ffeReg          = 0x0980028C + offset;
        addrPtr->serdesConfig[i].analogReservReg = 0x098003CC + offset;
        addrPtr->serdesConfig[i].digitalIfReg0   = 0x098003E0 + offset;
        addrPtr->serdesConfig[i].powerReg        = 0x09800368 + offset;
        addrPtr->serdesConfig[i].resetReg        = 0x0980036C + offset;
        addrPtr->serdesConfig[i].slcReg          = 0x0980035c + offset;
        addrPtr->serdesConfig[i].referenceReg    = 0x09800364 + offset;
        addrPtr->serdesConfig[i].standaloneCntrlReg = 0x098003F8 + offset;
        addrPtr->serdesConfig[i].serdesExternalReg1 = 0x09800000 + offset;
        addrPtr->serdesConfig[i].serdesExternalReg2 = 0x09800004 + offset;
        addrPtr->serdesConfig[i].phyTestReg0     = 0x09800378 + offset;
        addrPtr->serdesConfig[i].phyTestReg1     = 0x0980037C + offset;
        addrPtr->serdesConfig[i].phyTestDataReg5 = 0x09800394 + offset;
        addrPtr->serdesConfig[i].phyTestPrbsCntReg2    = 0x098003A0 + offset;
        addrPtr->serdesConfig[i].phyTestPrbsErrCntReg0 = 0x098003A4 + offset;
        addrPtr->serdesConfig[i].phyTestPrbsErrCntReg1 = 0x098003A8 + offset;

        if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
        {
            addrPtr->serdesConfig[i].serdesExternalReg3 = 0x0980000C + offset;
        }
    }
}

/*******************************************************************************
* xCatLedsRegsInit
*
* DESCRIPTION:
*       This function initializes the LED interface registers.
*
* INPUTS:
*       addrPtr     - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with IP regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCatLedRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    addrPtr->ledRegs.ledHyperGStackDebugSelect[1]   = 0x5005110;
    addrPtr->ledRegs.ledHyperGStackDebugSelect1[1]  = 0x5005114;
}

/*******************************************************************************
* xCatPreEgressEngineRegsInit
*
* DESCRIPTION:
*       This function initializes the registers bridge struct for devices that
*       belong to REGS_FAMILY_1 registers set. -- xCat
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with bridge regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCatPreEgressEngineRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC *addrPtr
)
{
    addrPtr->bufferMng.eqBlkCfgRegs.logicalTargetDeviceMappingConfReg = 0x0B050004;
}

#ifndef __AX_PLATFORM_XCAT__        
/*******************************************************************************
* xCatIpv4RegsInit
*
* DESCRIPTION:
*       This function initializes the IPv4 registers struct for
*       devices that belong to REGS_FAMILY_1 registers set. -- cheetah 2
*
* INPUTS:
*       addr    - pointer to registers struct
*
* OUTPUTS:
*       addr    - registers struct with IPv4 regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCatIpv4RegsInit
(
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addr
)
{
    addr->ipv4Regs.ctrlReg0 = 0x02800100;
    addr->ipv4Regs.ctrlReg1 = 0x02800104;
    addr->ipv4Regs.ucEnCtrlReg = 0x02800108;
    addr->ipv4Regs.mcEnCtrlReg = 0x0280010C;
}

/*******************************************************************************
* xCatIpv6RegsInit
*
* DESCRIPTION:
*       This function initializes the IPv6 registers struct for
*       devices that belong to REGS_FAMILY_1 registers set. -- cheetah 2
*
* INPUTS:
*       addr    - pointer to registers struct
*
* OUTPUTS:
*       addr    - registers struct with IPv6 regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCatIpv6RegsInit
(
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addr
)
{
    GT_U32  i;

    addr->ipv6Regs.ctrlReg0 = 0x02800200;
    addr->ipv6Regs.ctrlReg1 = 0x02800204;
    addr->ipv6Regs.ucEnCtrlReg = 0x02800208;
    addr->ipv6Regs.mcEnCtrlReg = 0x0280020C;

    for (i = 0; i < 4 ; i++)
    {
        addr->ipv6Regs.ucScopeTblRegs[i] = 0x02800278 + (i * 0x4);
        addr->ipv6Regs.mcScopeTblRegs[i] = 0x02800288 + (i * 0x4);
        addr->ipv6Regs.ipScopeRegs[i] = 0x02800250 + (i * 0x4);
        addr->ipv6Regs.ipScopeLevel[i] = 0x02800260 + (i * 0x4);
    }
}

#endif
/*******************************************************************************
* prvCpssDxChXcatHwRegAddrInit
*
* DESCRIPTION:
*       This function initializes the registers struct for DxCh xCat devices.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChXcatHwRegAddrInit
(
    IN GT_U32 devNum
)
{
    GT_STATUS rc;
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC *regsAddrPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);


    /* first set the "Cheetah-3" part */
    rc = prvCpssDxCh3HwRegAddrInit(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* set the specific "DXCH xcat 3" parts */
    regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    /* IPCL TCC - unit */
    xCatIpclTccUnitRegsInit(regsAddrPtr);

    /* IPvX TCC - unit */
    xCatIpvxTccUnitRegsInit(regsAddrPtr);

    /* MAC registers of 'gig stack ports'
       and SERDESs settings */
    xCatMacRegsInit(devNum,regsAddrPtr);

    /* L2 and L3 Port Isolation table settings */
    xCatEgrTxRegsInit(devNum,regsAddrPtr);
#ifndef __AX_PLATFORM_XCAT__
    xCatIpv4RegsInit(regsAddrPtr);
    xCatIpv6RegsInit(regsAddrPtr);
#endif
    xCatIpRegsInit(regsAddrPtr);

    xCatBrgRegsInit(devNum, regsAddrPtr);

    xCatIpMtRegsInit(devNum, regsAddrPtr);

    xCatPclRegsInit(regsAddrPtr);

    xCatTtiRegsInit(regsAddrPtr);

    xCatHaRegsInit(devNum, regsAddrPtr);

    xCatBridgeRegsInit(regsAddrPtr);

    xCatGlobalRegsInit(regsAddrPtr);

    xCatPolicerRegsInit(regsAddrPtr);

    xCatMirrorRegsInit(regsAddrPtr);

    xCatBufMngRegsInit(regsAddrPtr);

    xCatBcnRegsInit(devNum,regsAddrPtr);

    xCatPreEgressEngineRegsInit(regsAddrPtr);

    xCatSerdesRegsInit(devNum,regsAddrPtr);

    /* skip configuration for Lion */
    if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_LION_E)
    {
        xCatLedRegsInit(regsAddrPtr);
    }

    return GT_OK;
}

/*******************************************************************************
* lionSerdesRegsInit
*
* DESCRIPTION:
*       This function initializes the SERDES registers.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with bridge regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lionSerdesRegsInit
(
    IN GT_U32 devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC *addrPtr
)
{
    GT_U32 i, offset;

    devNum = devNum;
    for (i = 0; i < PRV_CPSS_LION_SERDES_NUM_CNS;i++)
    {
        offset = i * 0x400;
        addrPtr->serdesConfig[i].calibrationReg0 = 0x09800224 + offset;
        addrPtr->serdesConfig[i].calibrationReg1 = 0x0980022C + offset;
        addrPtr->serdesConfig[i].calibrationReg2 = 0x09800230 + offset;
        addrPtr->serdesConfig[i].calibrationReg3 = 0x09800234 + offset;
        addrPtr->serdesConfig[i].calibrationReg5 = 0x0980023C + offset;
        addrPtr->serdesConfig[i].calibrationReg7 = 0x09800228 + offset;
        addrPtr->serdesConfig[i].pllIntpReg1     = 0x0980020C + offset;
        addrPtr->serdesConfig[i].pllIntpReg2     = 0x09800210 + offset;
        addrPtr->serdesConfig[i].pllIntpReg3     = 0x09800214 + offset;
        addrPtr->serdesConfig[i].pllIntpReg4     = 0x09800218 + offset;
        addrPtr->serdesConfig[i].pllIntpReg5     = 0x0980021C + offset;
        addrPtr->serdesConfig[i].transmitterReg0 = 0x09800250 + offset;
        addrPtr->serdesConfig[i].transmitterReg1 = 0x09800254 + offset;
        addrPtr->serdesConfig[i].transmitterReg2 = 0x09800258 + offset;
        addrPtr->serdesConfig[i].receiverReg0    = 0x09800278 + offset;
        addrPtr->serdesConfig[i].receiverReg1    = 0x0980027C + offset;
        addrPtr->serdesConfig[i].receiverReg2    = 0x09800280 + offset;
        addrPtr->serdesConfig[i].ffeReg          = 0x0980028C + offset;
        addrPtr->serdesConfig[i].analogReservReg = 0x098003CC + offset;
        addrPtr->serdesConfig[i].digitalIfReg0   = 0x098003E0 + offset;
        addrPtr->serdesConfig[i].powerReg        = 0x09800368 + offset;
        addrPtr->serdesConfig[i].resetReg        = 0x0980036C + offset;
        addrPtr->serdesConfig[i].slcReg          = 0x0980035c + offset;
        addrPtr->serdesConfig[i].referenceReg    = 0x09800364 + offset;
        addrPtr->serdesConfig[i].standaloneCntrlReg = 0x098003F8 + offset;
        addrPtr->serdesConfig[i].serdesExternalReg1 = 0x09800000 + offset;
        addrPtr->serdesConfig[i].serdesExternalReg2 = 0x09800004 + offset;
        addrPtr->serdesConfig[i].serdesExternalReg3 = 0x0980000C + offset;
        addrPtr->serdesConfig[i].phyTestReg0     = 0x09800378 + offset;
        addrPtr->serdesConfig[i].phyTestReg1     = 0x0980037C + offset;
        addrPtr->serdesConfig[i].phyTestDataReg5 = 0x09800394 + offset;
        addrPtr->serdesConfig[i].phyTestPrbsCntReg2    = 0x098003A0 + offset;
        addrPtr->serdesConfig[i].phyTestPrbsErrCntReg0 = 0x098003A4 + offset;
        addrPtr->serdesConfig[i].phyTestPrbsErrCntReg1 = 0x098003A8 + offset;
    }
}

/*******************************************************************************
* lionTtiRegsInit
*
* DESCRIPTION:
*       This function initializes the TTI registers struct for Lion.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lionTtiRegsInit
(
    IN  GT_U32   devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32 baseAddr;

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        /* LionB0 */
        baseAddr = 0x01000000;
    }
    else
    {
        /* XCAT2 */
        baseAddr = 0x0C000000;
    }


    addrPtr->ttiRegs.globalUnitConfig    = baseAddr + 0x0;
    addrPtr->ttiRegs.pclIdConfig0        = baseAddr + 0x10;
    addrPtr->ttiRegs.pclIdConfig1        = baseAddr + 0x14;
    addrPtr->ttiRegs.ttiIpv4GreEthertype = baseAddr + 0x18;
    addrPtr->ttiRegs.ccfcEthertype       = baseAddr + 0x24;
    addrPtr->ttiRegs.specialEthertypes   = baseAddr + 0x28;
    addrPtr->ttiRegs.udeEthertypesBase   = baseAddr + 0xF0;
    addrPtr->ttiRegs.mplsEthertypes      = baseAddr + 0x30;
    addrPtr->ttiRegs.trunkHashCfg[0]     = baseAddr + 0x70;
    addrPtr->ttiRegs.trunkHashCfg[1]     = baseAddr + 0x74;
    addrPtr->ttiRegs.trunkHashCfg[2]     = baseAddr + 0x78;

    addrPtr->ttiRegs.ptpEthertypes       = baseAddr + 0x01A4;
    addrPtr->ttiRegs.ptpUdpDstPorts      = baseAddr + 0x01A8;
    addrPtr->ttiRegs.ptpPacketCmdCfg0Reg = baseAddr + 0x01AC;
    addrPtr->ttiRegs.ptpPacketCmdCfg1Reg = baseAddr + 0x01BC;
    addrPtr->ttiRegs.ptpGlobalConfig     = baseAddr + 0x2000;
    addrPtr->ttiRegs.ptpEnableTimeStamp  = baseAddr + 0x2004;

    PORTS_BMP_REG_SET_MAC(
        addrPtr->ttiRegs.ptpTimeStampPortEnReg,
        (baseAddr + 0x2008));

    addrPtr->ttiRegs.ptpGlobalFifoCurEntry[0]    = baseAddr + 0x2030;
    addrPtr->ttiRegs.ptpGlobalFifoCurEntry[1]    = baseAddr + 0x2034;
    addrPtr->ttiRegs.ptpTodCntrNanoSeconds       = baseAddr + 0x2010;
    addrPtr->ttiRegs.ptpTodCntrSeconds[0]        = baseAddr + 0x2014;
    addrPtr->ttiRegs.ptpTodCntrSeconds[1]        = baseAddr + 0x2018;
    addrPtr->ttiRegs.ptpTodCntrShadowNanoSeconds = baseAddr + 0x201C;
    addrPtr->ttiRegs.ptpTodCntrShadowSeconds[0]  = baseAddr + 0x2020;
    addrPtr->ttiRegs.ptpTodCntrShadowSeconds[1]  = baseAddr + 0x2024;

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        addrPtr->ttiRegs.ttiMiscConfig  = baseAddr + 0x200;
    }

    /***********************************************/
    /* next registers are actually in the TTI unit */
    /***********************************************/

    addrPtr->pclRegs.ipclTcpPortComparator0  = baseAddr + 0xA0;
    addrPtr->pclRegs.ipclUdpPortComparator0  = baseAddr + 0xC0;

    addrPtr->pclRegs.ttiUnitConfig           = baseAddr + 0x0;
    addrPtr->pclRegs.ttiEngineConfig         = baseAddr + 0x0C;

    addrPtr->pclRegs.fastStack               = baseAddr + 0x60;
    addrPtr->pclRegs.loopPortReg             = baseAddr + 0x64;

    addrPtr->bridgeRegs.vlanRegs.protoEncBased = baseAddr + 0x98;
    addrPtr->bridgeRegs.vlanRegs.protoEncBased1 = baseAddr + 0x9C ;

    /* 6 registers 0x01000080-0x01000094 for 12 protocols */
    addrPtr->bridgeRegs.vlanRegs.protoTypeBased = baseAddr + 0x80;

    addrPtr->bridgeRegs.qosProfileRegs.dscp2QosProfileMapTable = baseAddr + 0x0400;
    addrPtr->bridgeRegs.qosProfileRegs.up2QosProfileMapTable   = baseAddr + 0x0440;
    addrPtr->bridgeRegs.qosProfileRegs.exp2QosProfileMapTable  = baseAddr + 0x0460;
    addrPtr->bridgeRegs.qosProfileRegs.dscp2DscpMapTable       = baseAddr + 0x0900;

    /* IPv6 Solicited-Node Multicast Address Configuration/Mask Register0 */
    addrPtr->bridgeRegs.ipv6McastSolicitNodeAddrBase = baseAddr + 0x40;
    addrPtr->bridgeRegs.ipv6McastSolicitNodeMaskBase = baseAddr + 0x50;

    addrPtr->bridgeRegs.vntReg.vntCfmEtherTypeConfReg = baseAddr + 0x01C8;

    FORMULA_SINGLE_REG_MAC(
        addrPtr->bridgeRegs.bridgeIngressVlanEtherTypeTable,
        (baseAddr + 0x0300),0x4,4);

    FORMULA_SINGLE_REG_MAC(
        addrPtr->bridgeRegs.bridgeIngressVlanSelect,
        (baseAddr + 0x0310),0x4,32);

    FORMULA_SINGLE_REG_MAC(
        addrPtr->ttiRegs.qos.cfiUpToQoSProfileMappingTableSelector,
        (baseAddr + 0x0470),0x4,(PORTS_NUM_CNS / 4));

}

/*******************************************************************************
* lionHaRegsInit
*
* DESCRIPTION:
*       This function initializes the HA registers struct for Lion
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr - pointer to registers struct
*
* OUTPUTS:
*       addrPtr - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lionHaRegsInit
(
   IN GT_U32 devNum,
   INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32 baseAddr; /* Base Address */

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        /* Lion */
        baseAddr = 0x0E800000;
    }
    else
    {
        /* xCat2 */
        baseAddr = 0x0F000000;
    }

    addrPtr->haRegs.hdrAltMplsEthertypes = baseAddr + 0x0550;
    addrPtr->haRegs.hdrAltIEthertype     = baseAddr + 0x0554;

    addrPtr->haRegs.hdrAltGlobalConfig    = baseAddr + 0x0100;
    addrPtr->haRegs.hdrAltCnConfig        = baseAddr + 0x0420;
    addrPtr->haRegs.hdrAltCpidReg0        = baseAddr + 0x0430;
    addrPtr->haRegs.hdrAltCnTagFlowId     = baseAddr + 0x0428;
    addrPtr->haRegs.hdrAltCnmHeaderConfig = baseAddr + 0x0424;

    PORTS_BMP_REG_SET_MAC(
        addrPtr->haRegs.cscdHeadrInsrtConf,
        (baseAddr + 0x0004));
    PORTS_BMP_REG_SET_MAC(
        addrPtr->haRegs.eggrDSATagTypeConf,
        (baseAddr + 0x0020));

    PORTS_BMP_REG_SET_MAC(addrPtr->haRegs.invalidCrcModeConfigReg,
        (baseAddr + 0x0370));

    FORMULA_SINGLE_REG_MAC(
        addrPtr->haRegs.trunkNumConfigReg,
        (baseAddr + 0x002C),0x4,PORTS_NUM_CNS / 4);

    PORTS_BMP_REG_SET_MAC(
        addrPtr->haRegs.routerHdrAltEnMacSaModifyReg,
        (baseAddr + 0x0104));

    addrPtr->haRegs.routerMacSaBaseReg[0] = baseAddr + 0x010C;
    addrPtr->haRegs.routerMacSaBaseReg[1] = baseAddr + 0x0110;

    PORTS_BMP_REG_SET_MAC(
        addrPtr->haRegs.routerDevIdModifyEnReg,
        (baseAddr + 0x0114));

    FORMULA_SINGLE_REG_MAC(
        addrPtr->haRegs.routerHdrAltMacSaModifyMode,
        (baseAddr + 0x0120),0x4,PORTS_BMP_NUM_CNS*2);

    PORTS_BMP_REG_SET_MAC(
        addrPtr->haRegs.haVlanTransEnReg,
        (baseAddr + 0x0130));

    addrPtr->haRegs.qosProfile2ExpTable     = baseAddr + 0x0300;
    addrPtr->haRegs.qosProfile2DpTable      = baseAddr + 0x0340;

    PORTS_BMP_REG_SET_MAC(/* NOTE: this register not using local port like other registers in the HA */
        addrPtr->haRegs.toAnalyzerVlanAddConfig, (baseAddr + 0x0400));
    addrPtr->haRegs.ingAnalyzerVlanTagConfig = baseAddr + 0x0408;
    addrPtr->haRegs.egrAnalyzerVlanTagConfig = baseAddr + 0x040C;
    PORTS_BMP_REG_SET_MAC(
        addrPtr->haRegs.mirrorToAnalyzerHeaderConfReg,(baseAddr + 0x0080));

    FORMULA_SINGLE_REG_MAC(
        addrPtr->haRegs.bridgeEgressVlanEtherTypeTable,
        (baseAddr + 0x0500),0x4,TPID_NUM_CNS/2);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->haRegs.bridgeEgressPortTag0TpidSelect,
        (baseAddr + 0x0510),0x4,PORTS_NUM_CNS/8);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->haRegs.bridgeEgressPortTag1TpidSelect,
        (baseAddr + 0x0530),0x4,PORTS_NUM_CNS/8);

    addrPtr->haRegs.miscConfig = baseAddr + 0x0710;

    /* PTP */
    addrPtr->haRegs.ptpGlobalConfig = baseAddr + 0x0800;
    addrPtr->haRegs.ptpEnableTimeStamp = baseAddr + 0x0804;
    PORTS_BMP_REG_SET_MAC(
        addrPtr->haRegs.ptpTimeStampPortEnReg, (baseAddr + 0x0808));

    addrPtr->haRegs.ptpGlobalFifoCurEntry[0] = baseAddr + 0x0830;
    addrPtr->haRegs.ptpGlobalFifoCurEntry[1] = baseAddr + 0x0834;

    addrPtr->haRegs.ptpTodCntrNanoSeconds = baseAddr + 0x0810;
    addrPtr->haRegs.ptpTodCntrSeconds[0] = baseAddr + 0x0814;
    addrPtr->haRegs.ptpTodCntrSeconds[1] = baseAddr + 0x0818;
    addrPtr->haRegs.ptpTodCntrShadowNanoSeconds = baseAddr + 0x081C;
    addrPtr->haRegs.ptpTodCntrShadowSeconds[0] = baseAddr + 0x0820;
    addrPtr->haRegs.ptpTodCntrShadowSeconds[1] = baseAddr + 0x0824;

    FORMULA_SINGLE_REG_MAC(
        addrPtr->haRegs.keepVlan1Reg,
        (baseAddr + 0x0440),0x4,PORTS_NUM_CNS / 4);
}

/*******************************************************************************
* lionCutThroughRegsInit
*
* DESCRIPTION:
*       This function initializes the Cut Through registers.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       addrPtr     - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with bridge regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lionCutThroughRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    addrPtr->cutThroughRegs.ctEnablePerPortReg = 0x0F000010;
    addrPtr->cutThroughRegs.ctEthertypeReg = 0x0F000014;
    addrPtr->cutThroughRegs.ctUpEnableReg = 0x0F000018;
    addrPtr->cutThroughRegs.ctPacketIndentificationReg = 0x0F00001C;
    addrPtr->cutThroughRegs.ctCascadingPortReg = 0x0F000020;
    addrPtr->cutThroughRegs.ctCpuPortMemoryRateLimitThresholdReg = 0x0F800098;
    addrPtr->cutThroughRegs.ctGlobalConfigurationReg = 0x00000390;

    FORMULA_SINGLE_REG_MAC(
        addrPtr->cutThroughRegs.ctPortMemoryRateLimitThresholdReg,0x0F800080,0x4,6);
}

/*******************************************************************************
* lionPolicerRegsInit
*
* DESCRIPTION:
*       This function initializes the Policer registers
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr - pointer to registers struct
*
* OUTPUTS:
*       addrPtr - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lionPolicerRegsInit
(
   IN GT_U32          devNum,
   INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32  baseAddr[3]; /* IPRL0, IPRL1, EPRL*/
    GT_U32  ii;

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        /* for Lion */
        baseAddr[0] = 0x0C000000;
        baseAddr[1] = 0x00800000;
        baseAddr[2] = 0x07800000;
    }
    else
    {
        /* for XCAT2 */
        baseAddr[0] = 0x0C800000;
        baseAddr[1] = 0x0D000000;
        baseAddr[2] = 0x03800000;
    }

    for(ii = 0 ; ii < 3 ; ii++)
    {
        addrPtr->PLR[ii].policerControlReg            = baseAddr[ii] + 0x00000000; /*  0x0C000000  0x00800000 0x07800000 */
        addrPtr->PLR[ii].policerPortMeteringEnReg[0]  = baseAddr[ii] + 0x00000008; /*  0x0C000008  0x00800008 0x07800008 */
        addrPtr->PLR[ii].policerPortMeteringEnReg[1]  = baseAddr[ii] + 0x0000000C; /*  0x0C00000C  0x0080000C 0x0780000C */
        addrPtr->PLR[ii].policerMRUReg                = baseAddr[ii] + 0x00000010; /*  0x0C000010  0x00800010 0x07800010 */
        addrPtr->PLR[ii].policerErrorReg              = baseAddr[ii] + 0x00000050; /*  0x0C000050  0x00800050 0x07800050 */
        addrPtr->PLR[ii].policerErrorCntrReg          = baseAddr[ii] + 0x00000054; /*  0x0C000054  0x00800054 0x07800054 */
        addrPtr->PLR[ii].policerTblAccessControlReg   = baseAddr[ii] + 0x00000070; /*  0x0C000070  0x00800070 0x07800070 */
        addrPtr->PLR[ii].policerTblAccessDataReg      = baseAddr[ii] + 0x00000074; /*  0x0C000074  0x00800074 0x07800074 */
        addrPtr->PLR[ii].policerInitialDPReg          = baseAddr[ii] + 0x000000C0; /*  0x0C0000C0  0x008000C0 0x078000C0 */
        addrPtr->PLR[ii].policerIntCauseReg           = baseAddr[ii] + 0x00000100; /*  0x0C000100  0x00800100 0x07800100 */
        addrPtr->PLR[ii].policerIntMaskReg            = baseAddr[ii] + 0x00000104; /*  0x0C000104  0x00800104 0x07800104 */
        addrPtr->PLR[ii].policerShadowReg             = baseAddr[ii] + 0x00000108; /*  0x0C000108  0x00800108 0x07800108 */
        addrPtr->PLR[ii].policerControl1Reg           = baseAddr[ii] + 0x00000004; /*  0x0C000004  0x00800004 0x07800004 */
        addrPtr->PLR[ii].policerControl2Reg           = baseAddr[ii] + 0x0000002C; /*  0x0C00002C  0x0080002C 0x0780002C */
        addrPtr->PLR[ii].portMeteringPtrIndexReg      = baseAddr[ii] + 0x00001800; /*  0x0C001800  0x00801800 0x07801800 */

        /* PLR Tables */
        addrPtr->PLR[ii].policerMeteringCountingTbl   = baseAddr[ii] + 0x00040000; /*  0x0C040000  0x00840000 0x07840000 */
        addrPtr->PLR[ii].policerQosRemarkingTbl       = baseAddr[ii] + 0x00080000; /*  0x0C080000  0x00880000 0x07880000 */
        addrPtr->PLR[ii].policerManagementCntrsTbl    = baseAddr[ii] + 0x00000500; /*  0x0C000500  0x00800500 0x07800500 */
        addrPtr->PLR[ii].policerCountingTbl           = baseAddr[ii] + 0x00060000; /*  0x0C060000  0x00860000 0x07860000 */
        addrPtr->PLR[ii].policerTimerTbl              = baseAddr[ii] + 0x00000200; /*  0x0C000200  0x00800200 0x07800200 */
    }

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        /* for Lion */
        for(ii = 0 ; ii < 3 ; ii++)
        {
            addrPtr->PLR[ii].ipfixControl                 = baseAddr[ii] + 0x00000014; /*  0x0C000014  0x00800014 0x07800014 */
            addrPtr->PLR[ii].ipfixNanoTimerStampUpload    = baseAddr[ii] + 0x00000018; /*  0x0C000018  0x00800018 0x07800018 */
            addrPtr->PLR[ii].ipfixSecLsbTimerStampUpload  = baseAddr[ii] + 0x0000001C; /*  0x0C00001C  0x0080001C 0x0780001C */
            addrPtr->PLR[ii].ipfixSecMsbTimerStampUpload  = baseAddr[ii] + 0x00000020; /*  0x0C000020  0x00800020 0x07800020 */
            addrPtr->PLR[ii].ipfixDroppedWaThreshold      = baseAddr[ii] + 0x00000030; /*  0x0C000030  0x00800030 0x07800030 */
            addrPtr->PLR[ii].ipfixPacketWaThreshold       = baseAddr[ii] + 0x00000034; /*  0x0C000034  0x00800034 0x07800034 */
            addrPtr->PLR[ii].ipfixByteWaThresholdLsb      = baseAddr[ii] + 0x00000038; /*  0x0C000038  0x00800038 0x07800038 */
            addrPtr->PLR[ii].ipfixByteWaThresholdMsb      = baseAddr[ii] + 0x0000003C; /*  0x0C00003C  0x0080003C 0x0780003C */
            addrPtr->PLR[ii].ipfixSampleEntriesLog0       = baseAddr[ii] + 0x00000048; /*  0x0C000048  0x00800048 0x07800048 */
            addrPtr->PLR[ii].ipfixSampleEntriesLog1       = baseAddr[ii] + 0x0000004C; /*  0x0C00004C  0x0080004C 0x0780004C */
            addrPtr->PLR[ii].ipfixWrapAroundAlertTbl      = baseAddr[ii] + 0x00000800; /*  0x0C000800  0x00800800 0x07800800 */
            addrPtr->PLR[ii].ipfixAgingAlertTbl           = baseAddr[ii] + 0x00000900; /*  0x0C000900  0x00800900 0x07800900 */
        }
    }
    else
    {
        /* for XCAT2 */
        for(ii = 0 ; ii < 3 ; ii++)
        {
            addrPtr->PLR[ii].ipfixControl                 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
            addrPtr->PLR[ii].ipfixNanoTimerStampUpload    = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
            addrPtr->PLR[ii].ipfixSecLsbTimerStampUpload  = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
            addrPtr->PLR[ii].ipfixSecMsbTimerStampUpload  = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
            addrPtr->PLR[ii].ipfixDroppedWaThreshold      = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
            addrPtr->PLR[ii].ipfixPacketWaThreshold       = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
            addrPtr->PLR[ii].ipfixByteWaThresholdLsb      = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
            addrPtr->PLR[ii].ipfixByteWaThresholdMsb      = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
            addrPtr->PLR[ii].ipfixSampleEntriesLog0       = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
            addrPtr->PLR[ii].ipfixSampleEntriesLog1       = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
            addrPtr->PLR[ii].ipfixWrapAroundAlertTbl      = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
            addrPtr->PLR[ii].ipfixAgingAlertTbl           = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        }
    }
}

/*******************************************************************************
* lionGlobalRegsInit
*
* DESCRIPTION:
*       This function initializes the Global registers struct
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with buffer management regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lionGlobalRegsInit
(
    IN GT_U32                               devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    devNum = devNum;

    addrPtr->globalRegs.mppmPllConfig.mppmPllParams = 0x000000BC;
    addrPtr->globalRegs.mppmPllConfig.mppmPllCtrl   = 0x000000D0;
    addrPtr->globalRegs.chipId = 0x000000D4;

    addrPtr->globalRegs.globalControl4         = 0x00000364;
    addrPtr->globalRegs.globalControl3         = 0x00000368;
    addrPtr->globalRegs.tcamPllParams          = 0x0000036c;
    addrPtr->globalRegs.tcamPllCtrl            = 0x00000370;

    addrPtr->globalRegs.mediaInterface         = 0x0000038c;
    addrPtr->globalRegs.sdPllParams          = 0x00000374;
    addrPtr->globalRegs.sdPllCtrl            = 0x00000378;
}

/*******************************************************************************
* lionTxqRegsInit
*
* DESCRIPTION:
*       This function initializes the TXQ registers struct
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with buffer management regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lionTxqRegsInit
(
    IN GT_U32                               devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32  *tmpPtr;
    GT_U32  ii;
    GT_U32  size;

    devNum = devNum;

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* not relevant to 'TXQ rev 0'  devices */
        return ;
    }

    /* start by unsetting all the 'TXQ related' registers that MUST not be used
       for this device */
    {
        tmpPtr = (GT_U32*)(&addrPtr->egrTxQConf);
        size = sizeof(addrPtr->egrTxQConf) / sizeof(GT_U32);

        /***************************************/
        /* set 'old' TXQ registers as 'unused' */
        /***************************************/
        defaultAddressUnusedSet(tmpPtr,size);

        addrPtr->egrTxQConf.txPortRegs = NULL;/* avoid FREE error */
        addrPtr->egrTxQConf.swTxPortRegsBookmark = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
        addrPtr->egrTxQConf.swTxPortRegsType   = PRV_CPSS_SW_TYPE_PTR_FIELD_CNS;
        addrPtr->egrTxQConf.swTxPortRegsSize   = 0;

        /***************************************/
        /* set 'old' TXQ registers as 'unused' */
        /***************************************/
        addrPtr->bridgeRegs.egressFilter = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->bridgeRegs.egressFilter2 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->bridgeRegs.mcastLocalEnableConfig = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->bridgeRegs.egressFilterIpMcRouted = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->bridgeRegs.vntReg.vntOamLoopbackConfReg  = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->bridgeRegs.nstRegs.cpuPktsFrwFltConfReg = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->bridgeRegs.nstRegs.brgPktsFrwFltConfReg = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->bridgeRegs.nstRegs.routePktsFrwFltConfReg = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->bridgeRegs.srcIdReg.srcIdUcEgressFilterConfReg = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->bridgeRegs.egressFilterUnregBc = PRV_CPSS_SW_PTR_ENTRY_UNUSED;

        for(ii = 0; ii < 32; ii++)
        {
            addrPtr->bufferMng.devMapTbl[ii]       = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
            addrPtr->bufferMng.srcIdEggFltrTbl[ii] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        }

        addrPtr->bufferMng.trunkTblCfgRegs.baseAddrNonTrunk   = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->bufferMng.trunkTblCfgRegs.baseAddrTrunkDesig = PRV_CPSS_SW_PTR_ENTRY_UNUSED;

        addrPtr->interrupts.txqIntCause    = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->interrupts.txqIntMask     = PRV_CPSS_SW_PTR_ENTRY_UNUSED;

        addrPtr->macRegs.hgXsmiiCountersCtr = PRV_CPSS_SW_PTR_ENTRY_UNUSED;


        /* need to use table PRV_CPSS_DXCH_LION_TABLE_TXQ_EGRESS_STC_E
           instead of those registers */
        addrPtr->sFlowRegs.egrStcTblWord0 = NULL;/* avoid FREE error */
        addrPtr->sFlowRegs.swEgrStcTblW0Bookmark = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
        addrPtr->sFlowRegs.swEgrStcTblW0Type     = PRV_CPSS_SW_TYPE_PTR_FIELD_CNS;
        addrPtr->sFlowRegs.swEgrStcTblW0Size     = 0;

        addrPtr->sFlowRegs.egrStcTblWord1 = NULL;/* avoid FREE error */
        addrPtr->sFlowRegs.swEgrStcTblW1Bookmark = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
        addrPtr->sFlowRegs.swEgrStcTblW1Type     = PRV_CPSS_SW_TYPE_PTR_FIELD_CNS;
        addrPtr->sFlowRegs.swEgrStcTblW1Size     = 0;

        addrPtr->sFlowRegs.egrStcTblWord2 = NULL;/* avoid FREE error */
        addrPtr->sFlowRegs.swEgrStcTblW2Bookmark = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
        addrPtr->sFlowRegs.swEgrStcTblW2Type     = PRV_CPSS_SW_TYPE_PTR_FIELD_CNS;
        addrPtr->sFlowRegs.swEgrStcTblW2Size     = 0;
    }


    addrPtr->txqVer1.egr.global.cpuPortDistribution = 0x01800000;

    addrPtr->txqVer1.egr.mcFifoConfig.globalConfig  = 0x01801000;
    PORTS_BMP_REG_SET_MAC(
        addrPtr->txqVer1.egr.mcFifoConfig.distributionMask,0x01801010);

    addrPtr->txqVer1.egr.devMapTableConfig.addrConstruct  = 0x01801100;
    PORTS_BMP_REG_SET_MAC(
        addrPtr->txqVer1.egr.devMapTableConfig.localSrcPortMapOwnDevEn,0x01801110);
    PORTS_BMP_REG_SET_MAC(
        addrPtr->txqVer1.egr.devMapTableConfig.localTrgPortMapOwnDevEn,0x01801120);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.egr.devMapTableConfig.secondaryTargetPortTable,0x01801300,0x4,PORTS_NUM_CNS);

    addrPtr->txqVer1.egr.filterConfig.globalEnables  = 0x01810000;
    PORTS_BMP_REG_SET_MAC(
        addrPtr->txqVer1.egr.filterConfig.unknownUcFilterEn,0x01810010);
    PORTS_BMP_REG_SET_MAC(
        addrPtr->txqVer1.egr.filterConfig.unregisteredMcFilterEn,0x01810020);
    PORTS_BMP_REG_SET_MAC(
        addrPtr->txqVer1.egr.filterConfig.unregisteredBcFilterEn,0x01810030);
    PORTS_BMP_REG_SET_MAC(
        addrPtr->txqVer1.egr.filterConfig.mcLocalEn,0x01810040);
    PORTS_BMP_REG_SET_MAC(
        addrPtr->txqVer1.egr.filterConfig.ipmcRoutedFilterEn,0x01810050);
    PORTS_BMP_REG_SET_MAC(
        addrPtr->txqVer1.egr.filterConfig.ucSrcIdFilterEn,0x01810060);
    PORTS_BMP_REG_SET_MAC(
        addrPtr->txqVer1.egr.filterConfig.oamLoopbackFilterEn,0x01810070);
    PORTS_BMP_REG_SET_MAC(
        addrPtr->txqVer1.egr.filterConfig.egrCnEn,0x01810080);
    PORTS_BMP_REG_SET_MAC(
        addrPtr->txqVer1.egr.filterConfig.egrCnFcEn,0x01810090);

    addrPtr->txqVer1.queue.distributor.generalConfig  = 0x0A091000;
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.distributor.tcDpRemapping,0x0A091010,0x4,2);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.distributor.stackTcRemapping,0x0A091020,0x4,TC_NUM_CNS);
    PORTS_BMP_REG_SET_MAC(
        addrPtr->txqVer1.queue.distributor.stackRemapEn,0x0A091050);
    PORTS_BMP_REG_SET_MAC(
        addrPtr->txqVer1.queue.distributor.controlTcRemapEn,0x0A091060);
    PORTS_BMP_REG_SET_MAC(
        addrPtr->txqVer1.queue.distributor.dataTcRemapEn,0x0A091070);
    PORTS_BMP_REG_SET_MAC(
        addrPtr->txqVer1.queue.distributor.dpToCfiRemapEn,0x0A091080);
    PORTS_BMP_REG_SET_MAC(
        addrPtr->txqVer1.queue.distributor.fromCpuForwardRestricted,0x0A091090);
    PORTS_BMP_REG_SET_MAC(
        addrPtr->txqVer1.queue.distributor.bridgedForwardRestricted,0x0A0910a0);
    PORTS_BMP_REG_SET_MAC(
        addrPtr->txqVer1.queue.distributor.routedForwardRestricted,0x0A0910b0);

    addrPtr->txqVer1.queue.peripheralAccess.misc.misc  = 0x0A093000;
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.peripheralAccess.egressMibCounterSet.config,0x0A093200,0x4,2);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.peripheralAccess.egressMibCounterSet.outgoingUnicastPacketCounter,0x0A093210,0x4,EGRESS_COUNTERS_SETS_NUM_CNS);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.peripheralAccess.egressMibCounterSet.outgoingMulticastPacketCounter,0x0A093220,0x4,EGRESS_COUNTERS_SETS_NUM_CNS);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.peripheralAccess.egressMibCounterSet.outgoingBroadcastPacketCounter,0x0A093230,0x4,EGRESS_COUNTERS_SETS_NUM_CNS);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.peripheralAccess.egressMibCounterSet.bridgeEgressFilteredPacketCounter,0x0A093240,0x4,EGRESS_COUNTERS_SETS_NUM_CNS);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.peripheralAccess.egressMibCounterSet.tailDroppedPacketCounter,0x0A093250,0x4,EGRESS_COUNTERS_SETS_NUM_CNS);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.peripheralAccess.egressMibCounterSet.controlPacketCounter,0x0A093260,0x4,EGRESS_COUNTERS_SETS_NUM_CNS);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.peripheralAccess.egressMibCounterSet.egressForwardingRestrictionDroppedPacketsCounter,0x0A093270,0x4,EGRESS_COUNTERS_SETS_NUM_CNS);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.peripheralAccess.egressMibCounterSet.multicastFifoDroppedPacketsCounter,0x0A093280,0x4,EGRESS_COUNTERS_SETS_NUM_CNS);

    addrPtr->txqVer1.queue.peripheralAccess.cncModes.modesRegister  = 0x0A0936A0;

    addrPtr->txqVer1.queue.tailDrop.config.config = 0x0A0A0000;
    addrPtr->txqVer1.queue.tailDrop.config.byteCount = 0x0A0A0008;
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.config.portEnqueueEnable,0x0A0A0010,0x4,PORTS_NUM_CNS);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.config.tailDropCnProfile,0x0A0A0120,0x4,PORTS_NUM_CNS);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.config.tcProfileEnableSharedPoolUsage,0x0A0A0220,0x4,TC_NUM_CNS);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.config.profileEnableWrtdDp,0x0A0A0240,0x4,4);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.config.profilePriorityQueueToSharedPoolAssociation,0x0A0A0250,0x4,PROFILE_NUM_CNS);
    addrPtr->txqVer1.queue.tailDrop.config.wrtdMask0 = 0x0A0A0270;
    addrPtr->txqVer1.queue.tailDrop.config.wrtdMask1 = 0x0A0A0274;

    addrPtr->txqVer1.queue.tailDrop.limits.globalDescriptorsLimit = 0x0A0A0300;
    addrPtr->txqVer1.queue.tailDrop.limits.globalBufferLimit = 0x0A0A0310;
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.limits.profilePortLimits,0x0A0A0320,0x4,PROFILE_NUM_CNS);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.limits.sharedPoolLimits,0x0A0A0400,0x4,SHARED_POOLS_NUM_CNS);

    FORMULA_DOUBLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.limits.maximumQueueLimits,0x0A0A0500,
            0x20,PROFILE_NUM_CNS,0x4,TC_NUM_CNS);
    FORMULA_DOUBLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.limits.eqQueueLimitsDp0,0x0A0A0800,
            0x20,PROFILE_NUM_CNS,0x4,TC_NUM_CNS);
    FORMULA_DOUBLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.limits.dqQueueBufLimits,0x0A0A0900,
            0x20,PROFILE_NUM_CNS,0x4,TC_NUM_CNS);
    FORMULA_DOUBLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.limits.dqQueueDescLimits,0x0A0A0A00,
            0x20,PROFILE_NUM_CNS,0x4,TC_NUM_CNS);
    FORMULA_DOUBLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.limits.eqQueueLimitsDp1,0x0A0A0C00,
            0x20,PROFILE_NUM_CNS,0x4,TC_NUM_CNS);
    FORMULA_DOUBLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.limits.eqQueueLimitsDp2,0x0A0A0D00,
            0x20,PROFILE_NUM_CNS,0x4,TC_NUM_CNS);

    addrPtr->txqVer1.queue.tailDrop.counters.totalDescCounter = 0x0A0A1000;
    addrPtr->txqVer1.queue.tailDrop.counters.totalBuffersCounter = 0x0A0A1004;
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.counters.portDescCounter,0x0A0A1010,0x4,PORTS_NUM_CNS);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.counters.portBuffersCounter,0x0A0A1110,0x4,PORTS_NUM_CNS);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.counters.tcSharedDescCounter,0x0A0A1210,0x4,SHARED_POOLS_NUM_CNS);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.counters.tcSharedBuffersCounter,0x0A0A1250,0x4,SHARED_POOLS_NUM_CNS);
    /* There are 512 counters (lines) , selected by {port[5:0], prio[2:0]}*/
    FORMULA_DOUBLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.counters.qMainDesc,0x1088C000,
            0x20,PORTS_NUM_CNS,0x4,TC_NUM_CNS);
    FORMULA_DOUBLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.counters.qMainBuff,0x0A0A4000,
            0x20,PORTS_NUM_CNS,0x4,TC_NUM_CNS);

    addrPtr->txqVer1.queue.tailDrop.mcFilterLimits.mirroredPacketsToAnalyzerPortDescriptorsLimit = 0x0A0A3300;
    addrPtr->txqVer1.queue.tailDrop.mcFilterLimits.mirroredPacketsToAnalyzerPortBuffersLimit = 0x0A0A3310;
    addrPtr->txqVer1.queue.tailDrop.mcFilterLimits.multicastDescriptorsLimit = 0x0A0A3320;
    addrPtr->txqVer1.queue.tailDrop.mcFilterLimits.multicastBuffersLimit = 0x0A0A3330;

    addrPtr->txqVer1.queue.tailDrop.mcFilterCounters.snifferDescriptorsCounter = 0x0A0A6000;
    addrPtr->txqVer1.queue.tailDrop.mcFilterCounters.snifferBuffersCounter = 0x0A0A6004;
    addrPtr->txqVer1.queue.tailDrop.mcFilterCounters.multicastDescriptorsCounter = 0x0A0A6008;
    addrPtr->txqVer1.queue.tailDrop.mcFilterCounters.multicastBuffersCounter = 0x0A0A600C;

    addrPtr->txqVer1.queue.tailDrop.resourceHistogram.limitRegister1 = 0x0A0A6300;
    addrPtr->txqVer1.queue.tailDrop.resourceHistogram.limitRegister2 = 0x0A0A6304;
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.resourceHistogram.counters,0x0A0A6310,0x4,4);

    addrPtr->txqVer1.queue.congestNotification.cnGlobalConfigReg = 0x0A0B0000;
    addrPtr->txqVer1.queue.congestNotification.cnPrioQueueEnProfile = 0x0A0B0010;
    PORTS_BMP_REG_SET_MAC(
        addrPtr->txqVer1.queue.congestNotification.cnEnCnFrameTx,0x0A0B0020);
    PORTS_BMP_REG_SET_MAC(
        addrPtr->txqVer1.queue.congestNotification.cnFcEn,0x0A0B0030);
    addrPtr->txqVer1.queue.congestNotification.fbCalcConfigReg = 0x0A0B0040;
    addrPtr->txqVer1.queue.congestNotification.fbMinReg = 0x0A0B0050;
    addrPtr->txqVer1.queue.congestNotification.fbMaxReg = 0x0A0B0060;
    FORMULA_DOUBLE_REG_MAC(
        addrPtr->txqVer1.queue.congestNotification.cnProfileThreshold,0x0A0B0300,
            0x4,TC_NUM_CNS,0x20,PROFILE_NUM_CNS);
    addrPtr->txqVer1.queue.congestNotification.cnSampleTbl = 0x0A0B0090;

    addrPtr->txqVer1.queue.pfcRegs.pfcGlobalConfigReg = 0x0A0C0000;
    addrPtr->txqVer1.queue.pfcRegs.pfcSrcPortFcMode = 0x0A0C0010;
    addrPtr->txqVer1.queue.pfcRegs.pfcSourcePortProfile = 0x0A0C00B0;
    addrPtr->txqVer1.queue.pfcRegs.pfcGlobaGroupOfPortsThresholds = 0x0A0C0600;
    addrPtr->txqVer1.queue.pfcRegs.pfcProfileXoffThresholds = 0x0A0C0800;
    addrPtr->txqVer1.queue.pfcRegs.pfcProfileXonThresholds = 0x0A0C0A00;

    addrPtr->txqVer1.dq.global.config.config = 0x11000000;
    LION_16_LOCAL_PORTS_TO_64_GLOBAL_PORTS_REG_SET_MAC(
        addrPtr->txqVer1.dq.global.flushTrig.portTxqFlushTrigger,0x11000090,0x4);
    addrPtr->txqVer1.dq.global.debug.metalFix = 0x11000180;

    addrPtr->txqVer1.dq.scheduler.config.config = 0x11001000;
    addrPtr->txqVer1.dq.scheduler.config.portRequestMask = 0x11001004;
    addrPtr->txqVer1.dq.scheduler.config.schedulerByteCountForTailDrop = 0x11001008;
    LION_16_LOCAL_PORTS_TO_64_GLOBAL_PORTS_REG_SET_MAC(
        addrPtr->txqVer1.dq.scheduler.config.portRequestMaskSelector,0x1100100C,0x4);

    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.dq.scheduler.priorityArbiterWeights.profileSdwrrWeightsConfig0,
            0x11001140,0x4,PROFILE_NUM_CNS);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.dq.scheduler.priorityArbiterWeights.profileSdwrrWeightsConfig1,
            0x11001180,0x4,PROFILE_NUM_CNS);

    {
        FORMULA_SINGLE_REG_MAC(
            addrPtr->txqVer1.dq.scheduler.priorityArbiterWeights.profileSdwrrGroup,
                0x110011C0,0x10,4);

        FORMULA_SINGLE_REG_WITH_INDEX_MAC(
            addrPtr->txqVer1.dq.scheduler.priorityArbiterWeights.profileSdwrrGroup,
                0x110011C0 + 0xEC0,
                0x10,4,
                4);/* start index */
    }

    {
        FORMULA_SINGLE_REG_MAC(
            addrPtr->txqVer1.dq.scheduler.priorityArbiterWeights.profileSdwrrEnable,
                0x11001200,0x10,4);

        FORMULA_SINGLE_REG_WITH_INDEX_MAC(
            addrPtr->txqVer1.dq.scheduler.priorityArbiterWeights.profileSdwrrEnable,
                0x11001200 + 0xEC0,
                0x10,4,
                4);/* start index */
    }

    addrPtr->txqVer1.dq.scheduler.priorityArbiterWeights.portCpuSchedulerProfile = 0x11001240;
    LION_16_LOCAL_PORTS_TO_64_GLOBAL_PORTS_REG_SET_MAC(
        addrPtr->txqVer1.dq.scheduler.priorityArbiterWeights.portSchedulerProfile,
            0x11001280,0x4);

    addrPtr->txqVer1.dq.shaper.tokenBucketUpdateRate = 0x11002000;
    addrPtr->txqVer1.dq.shaper.tokenBucketBaseLine = 0x11002004;
    addrPtr->txqVer1.dq.shaper.cpuTokenBucketMtuConfig = 0x11002008;
    addrPtr->txqVer1.dq.shaper.portsTokenBucketMtuConfig = 0x1100200C;
    addrPtr->txqVer1.dq.shaper.tokenBucketMode = 0x11002010;
    LION_16_LOCAL_PORTS_TO_64_GLOBAL_PORTS_REG_SET_MAC(
        addrPtr->txqVer1.dq.shaper.portDequeueEnable,0x11002014,0x4);

    addrPtr->txqVer1.dq.flowControl.flowControlConfig = 0x11003000;
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.dq.flowControl.schedulerProfileLlfcMtu,0x11003008,0x4,PROFILE_NUM_CNS);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.dq.flowControl.pfcTimerToPriorityMapping,0x11003040,0x4,TC_NUM_CNS);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.dq.flowControl.schedulerProfileTcToPriorityMapEnable,0x11003060,0x4,PROFILE_NUM_CNS);

    FORMULA_DOUBLE_REG_MAC(
        addrPtr->txqVer1.dq.flowControl.tcProfilePfcFactor,0x11003080,
            0x20,TC_NUM_CNS,0x4,PROFILE_NUM_CNS);

    addrPtr->txqVer1.dq.statisticalAndCpuTriggeredEgressMirroringToAnalyzerPort.
        stcStatisticalTxSniffConfig = 0x11004000;
    addrPtr->txqVer1.dq.statisticalAndCpuTriggeredEgressMirroringToAnalyzerPort.
        egressCtmTrigger = 0x11004004;
    addrPtr->txqVer1.dq.statisticalAndCpuTriggeredEgressMirroringToAnalyzerPort.
        egressAnalyzerEnable = 0x11004010;

    addrPtr->txqVer1.sht.egressTables.deviceMapTableBaseAddress = 0x11870000;
    addrPtr->txqVer1.sht.global.shtGlobalConfig = 0x118F0000;

    addrPtr->txqVer1.sht.dfx.dfx0 = 0x118F0D80;
    addrPtr->txqVer1.sht.dfx.dfx1 = 0x118F0D84;
    addrPtr->txqVer1.sht.dfx.dfx2 = 0x118F0D88;
    addrPtr->txqVer1.sht.dfx.dfx3 = 0x118F0D8C;

    addrPtr->txqVer1.sht.dfx.dfxStatus0 = 0x118F0D90;

}


/*******************************************************************************
* lionTccUnitRegsInit
*
* DESCRIPTION:
*       This function initializes the IPvX,PCL registers struct for devices that
*       belong to REGS_FAMILY_1 registers set. -- lion B
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lionTccUnitRegsInit
(
    IN    GT_U32                            devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32      i;  /* loop iterator */

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* not relevant to 'TXQ rev 0'  devices */
        return ;
    }

    /***************************************************/
    /* those addresses are actually like in the Lion A */
    /* but we set them because xcat A1 has other values*/
    /***************************************************/

    /* IPCL TCC unit */
    addrPtr->pclRegs.actionTableBase      = 0x0d0B0000;
    addrPtr->pclRegs.tcamOperationTrigger = 0x0d000138;
    addrPtr->pclRegs.tcamOperationParam1  = 0x0d000130;
    addrPtr->pclRegs.tcamOperationParam2  = 0x0d000134;
    addrPtr->pclRegs.tcamReadPatternBase  = 0x0d040000;
    addrPtr->pclRegs.pclTcamConfig0       = 0x0d000208;

    /* IPvX TCC unit */
    addrPtr->ipRegs.routerTtTcamAccessCtrlReg      = 0x0d80041c;
    addrPtr->ipRegs.routerTtTcamAccessDataCtrlReg  = 0x0d800418;
    addrPtr->ipRegs.routerTtTcamAccessDataRegsBase = 0x0d800400;

    /* only base of all TCAM will be initialized */
    addrPtr->ipRegs.routerTtTcamBankAddr[0] = 0x0d840000;

    addrPtr->ipRegs.ipTcamConfig0       = 0x0d80096c;

    addrPtr->ipRegs.ipTcamControl = PRV_CPSS_SW_PTR_ENTRY_UNUSED;

    /* The following registers are not used in xCat */
    /* Tcam bank addresses, starting from the second one */
    for (i = 1; i <= 4; i++)
    {
        addrPtr->ipRegs.routerTtTcamBankAddr[i] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    }
}

/*******************************************************************************
* lionEpclRegsInit
*
* DESCRIPTION:
*       This function initializes lion specific EPCL registers addresses
*       to overlap the Cheetah3 registers addresses
*
* INPUTS:
*       devNum     - device number
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
static void lionEpclRegsInit
(
    IN    GT_U32                            devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32 addrBase; /* address base */

    /* EPCL registers */

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        /* Lion */
        addrBase = 0x0E000000;
    }
    else
    {
        /* xCat2 */
        addrBase = 0x0F800000;
    }

    addrPtr->pclRegs.epclGlobalConfig        = addrBase;

        /* bit per port registers */
    PORTS_BMP_REG_SET_MAC(
        addrPtr->pclRegs.epclCfgTblAccess, (addrBase + 0x10));
    PORTS_BMP_REG_SET_MAC(
        addrPtr->pclRegs.epclEnPktNonTs, (addrBase + 0x20));
    PORTS_BMP_REG_SET_MAC(
        addrPtr->pclRegs.epclEnPktTs, (addrBase + 0x30));
    PORTS_BMP_REG_SET_MAC(
        addrPtr->pclRegs.epclEnPktToCpu, (addrBase + 0x40));
    PORTS_BMP_REG_SET_MAC(
        addrPtr->pclRegs.epclEnPktFromCpuData, (addrBase + 0x50));
    PORTS_BMP_REG_SET_MAC(
        addrPtr->pclRegs.epclEnPktFromCpuControl, (addrBase + 0x60));
    PORTS_BMP_REG_SET_MAC(
        addrPtr->pclRegs.epclEnPktToAnalyzer, (addrBase + 0x70));

    /* TCP/UDP port EPCL comparators (8 register sequences) */
    addrPtr->pclRegs.epclTcpPortComparator0  = (addrBase + 0x100);
    addrPtr->pclRegs.epclUdpPortComparator0  = (addrBase + 0x140);
    addrPtr->pclRegs.eplrGlobalConfig = (addrBase + 0x0200);
}

/*******************************************************************************
* lionMllRegsInit
*
* DESCRIPTION:
*       This function initializes the MLL registers struct for
*       devices that belong to REGS_FAMILY_1 registers set . -- lion
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with IPv4 regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lionMllRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32 i;
    GT_U32 offset;

    addrPtr->ipMtRegs.mllGlobalReg = 0x0C800000;
    addrPtr->ipMtRegs.multiTargetRateShapingReg = 0x0C800210;
    addrPtr->ipMtRegs.mtUcSchedulerModeReg = 0x0C800214;
    addrPtr->ipMtRegs.mtTcQueuesSpEnableReg = 0x0C800200;
    addrPtr->ipMtRegs.mtTcQueuesWeightReg = 0x0C800204;
    addrPtr->ipMtRegs.mtTcQueuesGlobalReg = 0x0C800004;
    addrPtr->ipMtRegs.mllMcFifoFullDropCnt = 0x0C800984;
    for (i = 0 ; i < 8 ; i++)
    {
        addrPtr->ipMtRegs.qoSProfile2MTTCQueuesMapTable[i] = 0x0C800100 + 4 * i;
    }
    addrPtr->ipMtRegs.mllTableBase = 0x0C880000;
    for (i = 0 ; i < 2 ; i++)
    {
        offset = i * 0x100;
        addrPtr->ipMtRegs.mllOutInterfaceCfg[i]    = 0x0C800980 + offset;
        addrPtr->ipMtRegs.mllOutMcPktCnt[i]        = 0x0C800900 + offset;
    }
}

/*******************************************************************************
* lionCncRegsInit
*
* DESCRIPTION:
*       This function initializes the CNC registers struct for
*       devices that belong to REGS_FAMILY_1 registers set . -- lion
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lionCncRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    /* overwrite per couner block configuration */
    addrPtr->cncRegs.blockClientCfgAddrBase   = 0x08001180;

}

/*******************************************************************************
* lionMirrorRegsInit
*
* DESCRIPTION:
*       This function initializes the Global registers struct for
*       devices that belong to REGS_FAMILY_1 registers set. -- lion
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lionMirrorRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32   i;

    for (i = 3; i <= 5; i++)
    {
        addrPtr->bufferMng.eqBlkCfgRegs.portTxMirrorIndex[i] =
            0x0B00B044 + (i - 3) * 4;
    }

    addrPtr->bufferMng.eqBlkCfgRegs.portTxMirrorIndex[6] = 0x0B00B070;
}

/*******************************************************************************
* lionPclRegsInit
*
* DESCRIPTION:
*       This function initializes Lion specific PCL registers addresses
*       to overlap the xcat registers addresses , or to add new registers
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
static void lionPclRegsInit
(
    IN GT_U32          devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    addrPtr->pclRegs.ipclEngineInterruptCause = 0x0B800004;
    addrPtr->pclRegs.crcHashConfigurationReg = 0x0B800020;
    addrPtr->pclRegs.pclEngineExtUdbConfig   = 0x0B800024;
    FORMULA_SINGLE_REG_MAC(addrPtr->pclRegs.pearsonHashTableReg,0x0B8005C0 ,0x4,PEARSON_HASH_TABLE_NUM_CNS);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        /* policy TCAM Test - Lion but not XCAT2 */
        FORMULA_SINGLE_REG_MAC(
            addrPtr->pclRegs.policyTcamTest.pointerRelReg,0x0D000400 ,0x4,14);

        addrPtr->pclRegs.policyTcamTest.configStatusReg = 0x0D000450;
        addrPtr->pclRegs.policyTcamTest.opcodeCommandReg = 0x0D000454;
        addrPtr->pclRegs.policyTcamTest.opcodeExpectedHitReg = 0x0D000458;
    }
}

/*******************************************************************************
* prvCpssDxChLionHwRegAddrInit
*
* DESCRIPTION:
*       This function initializes the registers struct for DxCh Lion devices.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChLionHwRegAddrInit
(
    IN GT_U32 devNum
)
{
    GT_STATUS rc;
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC *regsAddrPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);


    regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    /* first set the "xCat" part */
    rc = prvCpssDxChXcatHwRegAddrInit(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* set the specific "DXCH Lion" parts */
    lionSerdesRegsInit(devNum,regsAddrPtr);
    lionTtiRegsInit(devNum,regsAddrPtr);
    lionPclRegsInit(devNum, regsAddrPtr);
    lionHaRegsInit(devNum,regsAddrPtr);
    lionIpRegsInit(devNum, regsAddrPtr);
    lionBufMngRegsInit(devNum, regsAddrPtr);
    lionCutThroughRegsInit(regsAddrPtr);
    lionPolicerRegsInit(devNum, regsAddrPtr);
    lionGlobalRegsInit(devNum, regsAddrPtr);
    lionTccUnitRegsInit(devNum, regsAddrPtr);
    lionMllRegsInit(regsAddrPtr);
    lionTxqRegsInit(devNum, regsAddrPtr);
    lionEpclRegsInit(devNum, regsAddrPtr);
    lionCncRegsInit(regsAddrPtr);
    lionMirrorRegsInit(regsAddrPtr);

    return GT_OK;
}
/*******************************************************************************
* xCat2BridgeRegsInit
*
* DESCRIPTION:
*       This function initializes the registers bridge struct for devices that
*       belong to REGS_FAMILY_1 registers set. -- xCat2
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with bridge regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCat2BridgeRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC *addrPtr
)
{
    addrPtr->bridgeRegs.miiSpeedGranularity   = 0x02040740;
    addrPtr->bridgeRegs.gmiiSpeedGranularity  = 0x02040744;
    addrPtr->bridgeRegs.stackSpeedGranularity = 0x02040748;
}


/*******************************************************************************
* xCat2DfxUnitsRegsInit
*
* DESCRIPTION:
*       This function initializes the registers DFX Units struct for devices that
*       belong to REGS_FAMILY_1 registers set. -- xCat2
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with bridge regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCat2DfxUnitsRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC *addrPtr
)
{
    addrPtr->dfxUnits.server.pipeSelect              = 0x008F8004;
    addrPtr->dfxUnits.server.temperatureSensorStatus = 0x008F8050;
    addrPtr->dfxUnits.server.resetControl            = 0x008F8014;
    addrPtr->dfxUnits.server.serverSkipInitMatrix    = 0x008F8024;
    addrPtr->dfxUnits.server.genSkipInitMatrix0      = 0x008F8028;
    addrPtr->dfxUnits.server.genSkipInitMatrix1      = 0x008F802C;
    addrPtr->dfxUnits.server.ledPadControl           = 0x008F8300;
    addrPtr->dfxUnits.server.gppPadControl           = 0x008F8304;
    addrPtr->dfxUnits.server.rgmiiPadControl         = 0x008F8310;
    addrPtr->dfxUnits.server.ssmiiPadControl         = 0x008F8314;

    addrPtr->dfxUnits.ingressLower.tcBist.tcamBistConfigStatus = 0x00804050;

    addrPtr->dfxUnits.ingressUpper.tcBist.tcamBistConfigStatus = 0x0080C050;
}

/*******************************************************************************
* xCat2TxqRegsInit
*
* DESCRIPTION:
*       This function initializes the TXQ registers -- xCat2
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with TXQ regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCat2TxqRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC *addrPtr
)
{
    GT_U8   i;

    addrPtr->egrTxQConf.queueWRTDMask = 0x01800230;
    addrPtr->egrTxQConf.portWRTDMask = 0x01800234;
    addrPtr->egrTxQConf.sharedPoolWRTDMask = 0x01800238;

    for (i = 0; i < PRV_CPSS_DXCH2_PROFILES_NUM_CNS; i++)
    {
        addrPtr->egrTxQConf.setsConfigRegs.portWRTDEn[i]       = 0x01800260 + i * 4;
        addrPtr->egrTxQConf.setsConfigRegs.queueWRTDEn[i]      = 0x018002A0 + i * 4;
        addrPtr->egrTxQConf.setsConfigRegs.sharedPoolWRTDEn[i] = 0x018002C0 + i * 4;
    }
}

/*******************************************************************************
* prvCpssDxChXcat2HwRegAddrInit
*
* DESCRIPTION:
*       This function initializes the registers struct for DxCh xCat2 devices.
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChXcat2HwRegAddrInit
(
    IN GT_U32 devNum
)
{
    GT_STATUS rc;
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC *regsAddrPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E);

    regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    /* first set the "xCat" part */
    rc = prvCpssDxChXcatHwRegAddrInit(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* set common Lion and xCat2 registers */
    lionTtiRegsInit(devNum,regsAddrPtr);
    lionPclRegsInit(devNum, regsAddrPtr);
    lionEpclRegsInit(devNum, regsAddrPtr);
    lionHaRegsInit(devNum,regsAddrPtr);
    lionBufMngRegsInit(devNum, regsAddrPtr);
    lionPolicerRegsInit(devNum, regsAddrPtr);
    lionCncRegsInit(regsAddrPtr);
    lionMirrorRegsInit(regsAddrPtr);

    /* set xCat2 specific registers */
    xCat2BridgeRegsInit(regsAddrPtr);
    xCat2DfxUnitsRegsInit(regsAddrPtr);
    xCat2TxqRegsInit(regsAddrPtr);
    xCat2PclRegsInit(regsAddrPtr);

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChXcatHwRegAddrRemove
*
* DESCRIPTION:
*       This function release the memory that was allocated by the function
*       prvCpssDxChXcatHwRegAddrInit(...)
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - The PP's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChXcatHwRegAddrRemove
(
    IN  GT_U32 devNum
)
{
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);


    /* first free the "Cheetah3" part */
    rc = prvCpssDxCh3HwRegAddrRemove(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* free the specific "xCat" parts */

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChXcat2HwRegAddrRemove
*
* DESCRIPTION:
*       This function release the memory that was allocated by the function
*       prvCpssDxChXcat2HwRegAddrInit(...)
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*       devNum  - The PP's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChXcat2HwRegAddrRemove
(
    IN  GT_U32 devNum
)
{
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E);


    /* first free the "xCat" part */
    rc = prvCpssDxChXcatHwRegAddrRemove(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* free the specific "xCat2" parts */

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLionHwRegAddrRemove
*
* DESCRIPTION:
*       This function release the memory that was allocated by the function
*       prvCpssDxChLionHwRegAddrInit(...)
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum  - The PP's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChLionHwRegAddrRemove
(
    IN  GT_U32 devNum
)
{
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);


    /* first free the "xCat" part */
    rc = prvCpssDxChXcatHwRegAddrRemove(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* free the specific "Lion" parts */

    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChXcatHwRegAddrPortMacUpdate
*
* DESCRIPTION:
*       This function updates mac registers for given port.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       ifMode   - new interface mode used with this MAC
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChXcatHwRegAddrPortMacUpdate
(
    IN GT_U32 devNum,
    IN GT_U32 portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode
)
{
    GT_U32 offset;   /* register offset */
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC *regsPtr;
    GT_U32 baseAddr;
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    /* convert the 'Physical port' to local port -- supporting multi-port-groups device */
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    offset = localPort * 0x400;
    regsPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) < PRV_CPSS_PORT_XG_E)
    {
        if(PRV_CPSS_PP_MAC(devNum)->revision == 0 &&
           PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E &&
           portNum >= 24)
        {
            baseAddr = 0x08800000;
        }
        else
        {
            baseAddr = 0x0A800000;/* xcat A1 and above */
        }


        /* Mac status registers     */
        regsPtr->macRegs.perPortRegs[portNum].macStatus = baseAddr + 0x10 + offset;
    }
    /* XG ports */
    else
    {
        if(ifMode == CPSS_PORT_INTERFACE_MODE_XLG_E)
        {
            offset = 0x3000;
        }

        if(ifMode == CPSS_PORT_INTERFACE_MODE_XLG_E)
        {
            regsPtr->macRegs.perPortRegs[portNum].miscConfig = 0x08800060 + offset;
        }
        else
        {
            regsPtr->macRegs.perPortRegs[portNum].miscConfig = 0x08800028 + offset;
        }

        /* Mac status registers     */
        regsPtr->macRegs.perPortRegs[portNum].macStatus = 0x0880000C + offset;
    }

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        /* MIB MAC counters */
        if (ifMode == CPSS_PORT_INTERFACE_MODE_XLG_E)
        {
            regsPtr->macRegs.perPortRegs[portNum].macCounters        =
                0x09180000;
        }
        else
        {
            offset = localPort * 0x20000;
            regsPtr->macRegs.perPortRegs[portNum].macCounters        =
                0x09000000 + offset;
        }

    }

    return GT_OK;
}

#ifdef _WIN32

#define SKIP_SPECIAL_ADDRESSES_MAC(address,index)           \
        if(address == PRV_CPSS_SW_PTR_ENTRY_UNUSED ||       \
           address == PRV_CPSS_SW_PTR_BOOKMARK_CNS)         \
        {                                                   \
            continue;                                       \
        }                                                   \
        if(regAddr == PRV_CPSS_SW_TYPE_PTR_FIELD_CNS)       \
        {                                                   \
            index++; /*skip also the pointer field */       \
            continue;                                       \
        }

/*******************************************************************************
* memoryChunkTest
*
* DESCRIPTION:
*       debug function for testing accessing register addresses in the TXQ
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum   - The PP's device number.
*       tmpPtr   - pointer to chunk of register addresses
*       size     - number of registers in chunk
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS   memoryChunkTest
(
    IN GT_U8    devNum,
    IN GT_U32  *tmpPtr,
    IN GT_U32   size
)
{
    GT_STATUS   rc;
    GT_U32  regAddr,regValue;
    GT_U32  ii;

    for(ii = 0 ; ii < size; ii++)
    {
        regAddr = tmpPtr[ii];

        SKIP_SPECIAL_ADDRESSES_MAC(regAddr,ii);

        /* write to the register it's address */
        rc = prvCpssDxChHwPpWriteRegister(devNum, regAddr, regAddr);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    for(ii = 0 ; ii < size; ii++)
    {
        regAddr = tmpPtr[ii];

        SKIP_SPECIAL_ADDRESSES_MAC(regAddr,ii);

        /* read the registers and check if value is as we set it */
        rc = prvCpssDxChHwPpReadRegister(devNum, regAddr, &regValue);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(regValue != regAddr)
        {
            cpssOsPrintf(" mismatch in reg[0x%8.8x] with value[0x%8.8x] \n",regAddr, regValue);
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChTxqVer1RegistersTest
*
* DESCRIPTION:
*       debug function for testing accessing register addresses in the TXQ
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS   prvCpssDxChTxqVer1RegistersTest
(
    IN GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC *addrPtr;
    GT_U32  *tmpPtr;
    GT_U32  size;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);


    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }

    addrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    tmpPtr = (GT_U32*)(&addrPtr->txqVer1);
    size = sizeof(addrPtr->txqVer1) / sizeof(GT_U32);
    memoryChunkTest(devNum,tmpPtr,size);

    tmpPtr = (GT_U32*)(&addrPtr->haRegs);
    size = sizeof(addrPtr->haRegs) / sizeof(GT_U32);
    memoryChunkTest(devNum,tmpPtr,size);

    return GT_OK;
}
#endif /*_WIN32*/

