/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell 
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the two
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or 
modify this File in accordance with the terms and conditions of the General 
Public License Version 2, June 1991 (the "GPL License"), a copy of which is 
available along with the File in the license.txt file or by writing to the Free 
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or 
on the worldwide web at http://www.gnu.org/licenses/gpl.txt. 

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED 
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY 
DISCLAIMED.  The GPL License provides additional details about this warranty 
disclaimer.
********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or 
modify this File under the following licensing terms. 
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    *   Redistributions of source code must retain the above copyright notice,
	    this list of conditions and the following disclaimer. 

    *   Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution. 

    *   Neither the name of Marvell nor the names of its contributors may be 
        used to endorse or promote products derived from this software without 
        specific prior written permission. 
    
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************************
* gtDrvPpRegs.h
*
* DESCRIPTION:
*       This file includes the declaration of the struct to hold the addresses
*       of PP registers for each different PP type.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __gtDrvPpRegsh
#define __gtDrvPpRegsh

/* SW_PTR_BOOKMARK      - This will used to indicate the spesific data types*/
/*                       (pointer fields or write only data) in PP_REGS_ADDR*/
/*                        struct                                            */
/* SW_PTR_ENTRY_UNUSED  - This will be used to mark pointer fields or       */
/*                        registers that not initialized in PP_REGS_ADDR    */
/*                        struct.                                           */

#define SW_PTR_BOOKMARK         0xFFFFFFFF
#define SW_PTR_ENTRY_UNUSED     0xFFFFFFFB

#define SW_TYPE_WRONLY          0x1     /* This will be used to mark write only data*/
                                        /*in PP_REGS_ADDR struct.*/
#define SW_TYPE_PFIELD          0x2     /* This will be used to mark dynamically allocated*/
                                        /*pointer fields in PP_REGS_ADDR struct.*/

#define LED_MAX_NUM_OF_INTERFACE 2
#define LED_MAX_NUM_OF_CLASS     7
#define LED_MAX_NUM_OF_GROUP     4

/*******************************************************************************
* driverPpRegsInit
*
* DESCRIPTION:
*       This function initializes the register's struct pointer in the given
*       device.
*
* INPUTS:
*       devNum  - The PP's device number to initialize.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
unsigned int driverPpRegsInit
(
	unsigned char devNum
);


/*
 * typedef: enum REGS_FAMILY_ID
 *
 * Description: This enumeration defines the different registers families that
 *              a given PP may be associated with.
 *
 * Fields:
 *      TWIST_REGS  - Identifies the following devices:
 *
 *
 *      SALSA_REGS  - Identifies the following devices:
 *                          -  DS_98DX240, DS_98DX241 ,DS_98DX160 ,DS_98DX161
 */
typedef enum
{
    TWIST_REGS,
    SAMBA_REGS,
    SALSA_REGS
}REGS_FAMILY;

struct _txPortRegs
{
    unsigned int txConfig;            /* Port Transmit Configuration  */
    unsigned int wdTxFlush;           /* Port Watchdog Register       */
    unsigned int tokenBuck;
    unsigned int tokenBuckLen;
    unsigned int prioTokenBuck[8];    /* per priority */
    unsigned int prioTokenBuckLen[8];    /* per priority */
    unsigned int wrrWeights0;         /* Port WRR Weights 0 Register  */
    unsigned int wrrWeights1;         /* Port WRR Weights 1 Register  */
    unsigned int wrrStrictPrio;       /* Port WRR & Strict Priority   */
    unsigned int descrLimit;          /* Port descriptor limit        */
    unsigned int tcQDescrLimit[12];    /* Port Traffic Class Queue Descriptors Limit 8 = Twist 12 = Salsa */

};

struct _perPortRegs
{
    unsigned int macCtrl;             /* MAC Control Register     */
    unsigned int macStatus;           /* MAC Status Register      */
    unsigned int macCounters;         /* MAC counters register.   */
    unsigned int macCounters2;        /* additional MAC cntr use for 10G port, */
                                /* get result by adding these 2 counters.*/
    unsigned int autoNegCtrl;         /* AutoNeg Control Register - Salsa only */
    unsigned int serdesCnfg;          /* Serdes configuration reg - Salsa only */
};

struct _bufMngPerPort
{
    unsigned int flowCntrlThr;        /* Port Flow Control Threshold Register */
    unsigned int rxBufLimit;          /* Port Receive Buffer Limit Register */
};

struct _nonTrunkTable
{
    unsigned int  nonTrunkMemLow;    /* non trunk members low */
    unsigned int  nonTrunkMemHigh;   /* non trunk members high*/
};

struct _trunkDesigTable
{
    unsigned int  trunkDesigPortsLow;     /* trunk designated ports low  */
    unsigned int  trunkDesigPortsHigh;    /* trunk designated ports high */
};

/*
 * Typedef: struct PP_REGS_ADDR
 *
 * Description: A struct including PP registers addresses for register access
 *              by different types of PPs.
 *
 * Fields: The struct includes fields by the names of PP registers, each
 *         field represents the respective register address.
 *
 * Notes:
 *      1.  Groups of registers (e.g: port(0,..9) control) will be represented
 *          by an array of registers addresses, for easy access to these
 *          registers.
 *      2.  Before each field which is write-only registers set and/or a pointer to a
 *          dynamically allocated buffer, bookmark is added to be used for scanning the
 *          PP_REGS_ADDR struct linearly, without depending on the names of the
 *          fields.
 *          The fields are:
 *              sw<FieldName>Bookmark - Indicates that a size field is
 *                                      declared two words ahead.
 *              sw<FieldName>Type     - Indicates the type of data followinf the bookmark:
 *                                      can be write only data and/or pointer field
 *              sw<FieldName>Size     - Indicates the size (in bytes) of the
 *                                      next field (which is a pointer).
 *                                      or/and number of bytes to skip in dump operation
 *                                      (if case then the data is write only)
 *          The above fields must appear immediately before every dynamically
 *          allocated pointer and in the above order.
 *
 */

/* NOTE:    the list of registers is not complete yet,  */
/*          it must be updated when the full registers  */
/*          tables are available.                       */
typedef struct
{
	
    struct _global
    {
        unsigned int          globalControl;  /* Global Control                      */
        unsigned int          globalStatus;   /* Global Status                       */
        unsigned int          addrCompletion; /* PCI to Internal memory regions map  */
        unsigned int          auQBaseAddr;    /* Addr Update Block Base address.     */
        unsigned int          auQControl;     /* Addr Update Queue Control.          */
        unsigned int          addrUpdate[4];  /* address update registers            */
        unsigned int          wideC2cCtrl;    /* Wide C2C Control Register           */
        unsigned int          narrowC2cCtrl;  /* Narrow C2C Control Register         */
        unsigned int          c2cStatus;      /* C2C status Register                 */
        unsigned int          dummyReadAfterWriteReg; /* Used for read after write   */
                                        /* limitation WA.                      */
        unsigned int          uplinkCfg;      /* uplink config register              */
        unsigned int          c2cArbCfg0;     /* Wide C2C Arbitration Configuration  */
                                        /* Register 0 Internal                 */
        unsigned int          c2cArbCfg1;     /* Wide C2C Arbitration Configuration  */
                                        /* Register 1 Internal                 */
        unsigned int          cpuPortCtrlReg; /* CPU port control register  - Salsa only */
        unsigned int          cpuPortStatusReg; /* CPU port status register - Salsa only */

    }globalRegs;

    struct _bufferMng
    {
        /* buffer management */
        unsigned int  buffMngParam;           /* Buffer Management Parameter      */
        unsigned int  localMaxBufReg;         /* Local Maximum Buffers Register   */
        unsigned int  localLinesLimitReg;     /* Local Lines limit Register       */
        unsigned int  uplinkMaxBuffers;       /* Uplink max buffers Register      */
        struct _eqBlkCfgRegs
        {
            unsigned int          rcvSniffMirror;
            unsigned int          trSrcSniff[8];
            unsigned int          trapSniffed;
        } eqBlkCfgRegs;

        struct _trunkTblCfgRegs
        {
            unsigned int  trunkMemTblBase;    /* Trunk Table Base */
            unsigned int  swNonTrunkBookmark;
            unsigned int  swNonTrunkType;
            unsigned int  swNonTrunkSize;
            struct _nonTrunkTable *nonTrunk; /* bit vectors per trunk group */
            unsigned int  swTrunkDesigBookmark;
            unsigned int  swTrunkDesigType;
            unsigned int  swTrunkDesigSize;
            struct _trunkDesigTable *trunkDesig;
        } trunkTblCfgRegs;

        unsigned int  swBufMngPerPortBookmark;
        unsigned int  swBufMngPerPortType;
        unsigned int  swBufMngPerPortSize;
        struct _bufMngPerPort *bufMngPerPort;   /* array of per port regs */
        unsigned int  cpuRxBufLimit;          /* CPU Receive Buffer Limit Reg */
        unsigned int  cpuRxBufCount;          /* CPU Receive Buffer Limit Cnt */

        unsigned int  vlanTblConfig;          /* Vlan and Tunnel memory size config*/
        unsigned int  epfGlobalControl;       /* EPF Global Control reg       */
        unsigned int  rxFifoCfgBase;          /* RX FIFO Config base (0 - 5)  */

    }bufferMng;

    struct _egrTxQConf              /* Egress and TxQ Configuration */
    {
        struct _setsConfigRegs
        {
            unsigned int redQWeight[4];       /* Set RED Queue Weight Register */
            unsigned int tcDp0Red[8][4];      /* Set Traffic Class DP 0 RED */
                                        /* 8 traffic classes and 4 sets */
            unsigned int tcDp1Red[8][4];      /* Set Traffic Class DP 1 RED */
            unsigned int tcDp2Red[8][4];      /* Set Traffic Class DP 2 RED */
            unsigned int tcRedParams0[8][4];  /* Set Traffic Class RED Parameter 0 */
            unsigned int tcRedParams1[8][4];  /* Set Traffic Class RED Parameter 1 */

        }setsConfigRegs;

        struct _txQCountSet         /* Transmit Queue Count SET 0,1 */
        {
            unsigned int  txQConfig;      /* Configuration Register                */
            unsigned int  outUcPkts;      /* Outgoing Unicast Packet Count         */
            unsigned int  outMcPkts;      /* Outgoing Multicast Packet Count       */
            unsigned int  outBytePkts;    /* Outgoing Byte Count Packet Count      */
            unsigned int  brgEgrFiltered; /* Bridge Egress Filtered Packet Count   */
            unsigned int  txQFiltered;    /* Transmit Queue Filtered Packet Count  */

        }txQCountSet[2];

        unsigned int     trSniffMirrorReg;    /* Transmit Sniffer Register    */
        unsigned int     txQueueConfig;       /* Transmit Queue Control       */
        unsigned int     txRouterEgressConfig;    /* Router Egress Configuration*/

        struct _txPortRegs  txCpuRegs;  /* CPU Tx regs                  */
        unsigned int  swTxPortRegsBookmark;
        unsigned int  swTxPortRegsType;
        unsigned int  swTxPortRegsSize;
        struct _txPortRegs *txPortRegs; /* ptr to Tx regs struct for ports */
    }egrTxQConf;

    struct _interrupts
    {
        unsigned int  intSumCause;        /* PCI Interrupt Summary Cause */
        unsigned int  intSumMask;         /* PCI Interrupt Summary Mask  */
        unsigned int  ethBrgCause;        /* Ethernet Bridge Interrupt Cause */
        unsigned int  ethBrgMask;         /* Ethernet Bridge Interrupt Mask Cause */
        unsigned int  lxIntCause;         /* L3-L7 Interrupt Cause */
        unsigned int  lxIntMask;          /* L3-L7 Interrupt Mask */
        unsigned int  bmIntCause;         /* EPF Buffer Management Interrupt Cause */
        unsigned int  bmIntMask;          /* EPF Buffer Management Interrupt Mask */
        unsigned int  gopInt0Cause;       /* GOP Interrupt Cause Register 0 */
        unsigned int  gopInt0Mask;        /* GOP Interrupt Mask Register 0 */
        unsigned int  gopInt1Cause;       /* GOP Interrupt Cause Register 1 */
        unsigned int  gopInt1Mask;        /* GOP Interrupt Mask Register 1 */
        unsigned int  miscIntCause;       /* Miscellaneous Interrupt Cause */
        unsigned int  miscIntMask;        /* Miscellaneous Interrupt Mask  */
        unsigned int  txqIntCause;        /* Transmit Queue Interrupt Cause */
        unsigned int  txqIntMask;         /* Transmit Queue Interrupt Mask */
        unsigned int  rxDmaIntCause;      /* Receive SDMA Interrupt Cause */
        unsigned int  rxDmaIntMask;       /* Receive SDMA Interrupt Mask  */
        unsigned int  txDmaIntCause;      /* Transmit SDMA Interrupt Cause */
        unsigned int  txDmaIntMask;       /* Transmit SDMA Interrupt Mask */
    }interrupts;

    unsigned int  swi2cRegBookmark;
    unsigned int  swi2cRegType;
    unsigned int  swi2cRegSize;
    struct _i2cReg
    {
          unsigned int  slaveAddr;
          unsigned int  slaveAddrExtend;
          unsigned int  data;
          unsigned int  ctrl;
          unsigned int  status;
          unsigned int  baudRate;
          unsigned int  softReset;
          unsigned int  serInitCtrl;
          unsigned int  serInitStatus;
          unsigned int  serFinishAddr;
    }i2cReg;

    struct _sdmaRegs
    {
        unsigned int  sdmaCfgReg;         /* SDMA configuration register.         */
        unsigned int  rxQCmdReg;          /* Receive Queue Command Reg.           */
        unsigned int  rxDmaCdp[8];        /* Rx SDMA Current Rx Desc. Pointer Reg */
        unsigned int  txQCmdReg;          /* Transmit Queue Command Reg.          */
        unsigned int  txDmaCdp[8];        /* Tx Current Desc Pointer Reg.         */
        unsigned int  rxDmaResErrCnt[2];  /* Rx SDMA resource error count regs0/1.*/
        unsigned int  rxDmaPcktCnt[8];    /* Rx SDMA packet count.                */
        unsigned int  rxDmaByteCnt[8];    /* Rx SDMA byte count.                  */
        unsigned int  txQWrrPrioConfig[8];/* Tx Queue WRR Priority Configuration. */
        unsigned int  txQFixedPrioConfig; /* Tx Queue Fixed Prio Config Register. */
    }sdmaRegs;

    struct _macRegs                 /* mac related */
    {
        unsigned int  xgmiiParams;        /* XGMII Parameters */
        unsigned int  xgmiiStatus;        /* XGMII Status     */

        unsigned int  pppBrgControl;      /* PPP Bridging Control */
        unsigned int  srcAddrMiddle;      /* Source Address Middle Register       */
        unsigned int  srcAddrHigh;        /* Source Address High Register         */

        struct _gopPppRegs
        {
            unsigned int  gopControl;         /* GOP 0/1 control Register */
            unsigned int  txThreshold;        /* GOP 0/1 GE Tx Threshold Register */
            unsigned int  txThresholdFe;      /* GOP 0/1 FE Tx Threshold Register */
            unsigned int  pppType[4];         /* Programmable PPP Type Registers */
            unsigned int  pppPrgAction[2];    /* Programmable PPP Action Register */
            unsigned int  pppPredAction[3];   /* Predefined PPP Action Register */
            unsigned int  pppDefaultAct;      /* Default PPP Action Register */
        }gopPppRegs[2];

        struct _gopRegs
        {
            unsigned int  srcAddrMiddle;      /* Source Address Middle Register - Salsa only */
            unsigned int  srcAddrHigh;        /* Source Address High Register   - Salsa only */
            unsigned int  gopControl;         /* GOP 0/1 control Register         */
            unsigned int  txThreshold;        /* GOP 0/1 GE Tx Threshold Register */
            unsigned int  txThresholdFe;      /* GOP 0/1 FE Tx Threshold Register */
        }gopRegs[4];

        unsigned int  swPerPortRegsBookmark;
        unsigned int  swPerPortRegsType;
        unsigned int  swPerPortRegsSize;
        unsigned int  macCountersCtrl[4];  /* MAC counters control register. - Salsa only */
        struct _perPortRegs *perPortRegs; /* MAC regs for ports */

    }macRegs;

    struct _ledRegs
    {
        unsigned int  ledControl[LED_MAX_NUM_OF_INTERFACE];
        unsigned int  ledClass  [LED_MAX_NUM_OF_INTERFACE][LED_MAX_NUM_OF_CLASS];
        unsigned int  ledGroup  [LED_MAX_NUM_OF_INTERFACE][LED_MAX_NUM_OF_GROUP];
        unsigned int  ledParams1;             /* Led parameters register interface1. */
        unsigned int  ledParams2;             /* Led parameters register interface2. */
        unsigned int  ledOverride;            /* Led override register.   */
    }ledRegs;

    struct _smiRegs
    {
        unsigned int smi10GeControl;          /* 10GE SMI Control Register */
        unsigned int smi10GeAddr;             /* 10GE SMI Address Register */
        unsigned int smiControl;              /* SMI Control Register      */
        unsigned int smiPhyAddr;              /* PHY Address Register0 */

    }smiRegs;

    struct _bridge
    {
        unsigned int      swPortControlBookmark;
        unsigned int      swPortControlType;
        unsigned int      swPortControlSize;
        unsigned int      *portControl;    /* Bridge Ports Control */
        unsigned int      cpuPortControl;  /* Bridge CPU port Control - Salsa only */
        unsigned int      swPortVidBookmark;
        unsigned int      swPortVidType;
        unsigned int      swPortVidSize;
        unsigned int      *portVid;        /* Ports Vid */
        unsigned int      swBrdByteLimitBookmark;
        unsigned int      swBrdByteLimitType;
        unsigned int      swBrdByteLimitSize;
        unsigned int      *brdByteLimit;   /* Ports Broadcast Byte Count Limit */
        unsigned int      swBrdByteWindowBookmark;
        unsigned int      swBrdByteWindowType;
        unsigned int      swBrdByteWindowSize;
        unsigned int      *brdByteWindow;  /* Ports Broadcast Byte Count Window */
        unsigned int      l2IngressControl;   /* Layer 2 Ingress Control */
        unsigned int      unknRateLimit0;     /* Unknown RL 0 */
        unsigned int      unknRateLimit1;     /* Unknown RL 1 */
        unsigned int      macTblAction0;      /* MAC Table Action0 */
        unsigned int      macTblAction1;      /* MAC Table Action1  - Salsa only */
        unsigned int      macTblAction2;      /* MAC Table Action2  - Salsa only */
        unsigned int      macRangeFltrCmd;    /* MAC Range Filter Command */
        unsigned int      macRangeFltMirrorCmd; /* MAC Range Filter Mirror Command */
        unsigned int      swMacRangeFltrBaseBookmark;
        unsigned int      swMacRangeFltrBaseType;
        unsigned int      swMacRangeFltrBaseSize;
        unsigned int      *macRangeFltrBase;  /* MAC Range Filtering bases */
        unsigned int      deviceTable[4];     /* Device Table Register */
        unsigned int      trunkTable[4];      /* Number of ports in each trunk*/
        unsigned int      brgMacCntr0;        /* base address for MAC Addr Count0-2*/
        unsigned int      hostInPckt;         /* Host Incoming Packets Count */
        unsigned int      hostOutPckt;        /* Host Outgoing Packets Count */
        unsigned int      hostInOct;          /* Host Incoming Octets Count */
        unsigned int      hostOutOct;         /* Host Outgoing Octet Count */
        unsigned int      hostOutBrdPckt;     /* Host Outgoing Broadcast Pckt Count */
        unsigned int      hostOutMcPckt;      /* Host Outgoing Multicast Pckt Count */
        unsigned int      matrixPckt;         /* Matrix Src/Dst Packet Count */
        unsigned int      matrixOct;          /* Matrix Src/Dst Octet Count */
        unsigned int      learnedDisc;        /* Learned Entry Discards Count */
        unsigned int      vlan0;              /* VLAN0 Register */
        unsigned int      vlan1;              /* VLAN1 Register */
        unsigned int      ingFltStatus;       /* Ingress Filter */
        unsigned int      egressFilter;       /* Egress Bridging */
        unsigned int      egressFilter2;      /* Egress Bridging 2 register(Twist)*/
        unsigned int      port52ConfigReg;    /* Egress and Tx Queue config reg */
        unsigned int      macControlReg;      /* MAC Table Control Register */
        unsigned int      mcFifoCtrlReg;      /* MC Fifo control register */


        struct _brgCntrSet
        {
            unsigned int  cntrSetCfg;         /* Counter Set Configuration */
            unsigned int  inPckt;             /* Incoming Packet Count */
            unsigned int  inFltPckt;          /* Ingress Filtered Packet Count */
            unsigned int  secFltPckt;         /* Security Filtered Packet Count */
            unsigned int  brgFltPckt;         /* Bridge Filtered Packet Count */
        }brgCntrSet[4];

        struct _cosConfRegs
        {
            unsigned int      userPrio2TrfClass;
            unsigned int      trfClass2UserPrio;
            unsigned int      userPrio2DropPrec;
            unsigned int      dscp2cos;
            unsigned int      forceL3cos;
            unsigned int      cpuCodeForceL3cos;  /* Salsa only */
      } cosConfRegs;

        struct _vlanRegs
        {
            unsigned int      subnetIpBased;      /* Subnet IP */
            unsigned int      subnetMaskBased;    /* Subnet Mask */
            unsigned int      subnetVidBased;     /* Subnet Vid */
            unsigned int      swProtoVidBasedBookmark;
            unsigned int      swProtoVidBasedType;
            unsigned int      swProtoVidBasedSize;
            unsigned int      *protoVidBased;     /* Ports Protocol Vid */
            unsigned int      protoTypeBased;     /* Protocols */
            unsigned int      protoEncBased;      /* Protocols Encapsulation */
            unsigned int      swPortPrvVlanBookmark;
            unsigned int      swPortPrvVlanType;
            unsigned int      swPortPrvVlanSize;
            unsigned int      *portPrvVlan;       /* Ports PVLAN */
        }vlanRegs;

    }bridgeRegs;

    struct _mplsRegs
    {
        unsigned int  ctrlReg;
        unsigned int  reservedLabelBase;
        unsigned int  labelIfBase;
        unsigned int  tcbRedirectBase;
        unsigned int  errorMplsEntry;
        unsigned int  defPhpCmdReg;
        unsigned int  exp2CosTable;
        unsigned int  counterSet[3];
        unsigned int  counterShadowReg;
        unsigned int  globalCounter;
    }mplsRegs;

    struct _flowRegs
    {
        unsigned int  ftCfg;              /* Flow Template Configuration Register */
        unsigned int  ftHashCfg;          /* Flow Template Hash Configuration Reg */
        unsigned int  ftSelectTable;      /* Flow Template Select Table Register  */
        unsigned int  ftHashSelectTable;  /* Flow Template Hash Select Table Reg  */
        unsigned int  defaultCmdBase;     /* Default Key Entry Command Table      */
    }flowRegs;

    struct _pcl
    {
        unsigned int  control;            /* PCL control                          */
        unsigned int  errorAddr;          /* PCL error register                   */
        unsigned int  ftCfg;              /* PCL - ft Configuration Register      */
        unsigned int  ftHashCfg;          /* PCL - ft Hash Configuration Reg      */
        unsigned int  ftSelectTable;      /* PCL - ft Select Table Register       */
        unsigned int  ftHashSelectTable;  /* PCL - ft Hash Select Table Reg       */
        unsigned int  defaultAction;      /* PCL - Default Key Entry Command Table*/
        unsigned int  tcamFlush ;         /* PCL - (samba only)) */
                                    /* flushes all TCAM memory (PCEs) */
        unsigned int  actonLookupMap ;    /* merge actions of two lookups */
        unsigned int  pclDefaultAction ;  /* action for packets */
                                    /* not matched by all PCEs */
    }pclRegs;

    struct _ipv4Regs
    {
        unsigned int  ipTunnelTerm1;      /* IP tunnel termination register 1.    */
        unsigned int  vrTableBase;        /* Base address of the Vr Table.        */
        unsigned int  defMcTblBase;       /* Base address of the Default Mc Tbl.  */
        unsigned int  mllBaseAddr;        /* Base address of Mll memory block.    */
        unsigned int  ipv4CtrlReg;        /* IPv4 Global control register.        */
        unsigned int  rxPcktCnt[3];       /* Receive Packet Counter Table Register*/
        unsigned int  rxOctetCntLow[3];   /* Receive Octet Counter Low Register   */
        unsigned int  rxOctetCntHigh[3];  /* Receive Octet Counter High Register  */
        unsigned int  discardCnt[3];      /* Discard Packet Counter Register      */
        unsigned int  discardOctet[3];    /* Discard Octet Counter Register       */
        unsigned int  ipMcPcktCnt[3];     /* Ip-Mc packets Counter Register       */
        unsigned int  trapPcktCnt[3];     /* Trapped Packet Counter Register      */
        unsigned int  tunnelTermPcktCnt[3];/* Tunnel Terminate Packet Counter Reg.*/
        unsigned int  failRpfPcktCnt[3];  /* Failed Reverse Path Forwarding Packet*/
                                    /* Counter Register.                    */
        unsigned int  ipGlobalCntReg0;    /* IPv4 Global Counter 0 Register.      */
        unsigned int  ipGlobalCntReg1;    /* IPv4 Global Counter 1 Register.      */
        unsigned int  ipGlobalCntReg2;    /* IPv4 Global Counter 2 Register.      */
        unsigned int  ipGlobalDiscardPcktCnt; /* IPv4 Discard Packet Counter Reg. */
        unsigned int  ipGlbDiscardOctetCntLow;/* IPv4 Discard Octet Low Counter   */
                                    /* Register.                            */
        unsigned int  ipGlbDiscardOctetCntHigh;/* IPv4 Discard Octet Low Counter  */
                                    /* Register.                            */
        unsigned int  ipv4ShadowCntReg;   /* IPv4 Shadow Counter Register.        */
        unsigned int  dscp2CosTable;      /* dscp to cos table                    */
    }ipv4Regs;

    struct _extMemRegs
    {
        /* Dram configuration registers     */
        unsigned int  fdOpenPagesReg;     /* DxB Open Pages Register for Flow Dram*/
        unsigned int  bdOpenPagesReg;     /* DxB Open Pages Register for Buff Dram*/
        unsigned int  fdModeReg;          /* DxB Mode Register for Flow Dram.     */
        unsigned int  bdModeReg;          /* DxB Mode Register for Buff Dram.     */
        unsigned int  fdExtModeReg;       /* DxB extend Mode Register Flow Dram.  */
        unsigned int  bdExtModeReg;       /* DxB extend Mode Register Buff Dram.  */
        unsigned int  fdTimingReg;        /* DxB timing Register flow Dram.       */
        unsigned int  bdTimingReg;        /* DxB timing Register Buff Dram.       */
        unsigned int  fdTiming2Reg;       /* DxB timing2 Register flow Dram.      */
        unsigned int  bdTiming2Reg;       /* DxB timing2 Register Buff Dram.      */
        unsigned int  fdTiming3Reg;       /* DxB timing3 Register flow Dram.      */
        unsigned int  bdTiming3Reg;       /* DxB timing3 Register Buff Dram.      */
        unsigned int  bdTiming4Reg;       /* DxB timing4 Register Buff Dram.      */
        unsigned int  fdConfig1Reg;       /* DxB Config Register for Flow Dram.   */
        unsigned int  bdConfig1Reg;       /* DxB Config Register for Buff Dram.   */
        unsigned int  fdConfig2Reg;       /* DxB Config2 Register for Flow Dram.  */
        unsigned int  bdConfig2Reg;       /* DxB Config2 Register for Buff Dram.  */
        unsigned int  fdOprationReg;      /* DxB Operation Register for Flow Dram.*/
        unsigned int  bdOprationReg;      /* DxB Operation Register for Buff Dram.*/
        unsigned int  fdAddressReg;       /* DxB Address register for Flow Dram.  */
        unsigned int  bdAddressReg;       /* DxB Address register for Buff Dram.  */
        unsigned int  bdRdWrMasterReg;    /* DxB Rd Wr Master Params Buff Dram.   */
        unsigned int  bdBufferThrReg;     /* DxB Buffer Thresholds for Buff Dram. */
        unsigned int  fdDataReg;          /* DxB Data register for Flow Dram.     */
        unsigned int  bdDataReg;          /* DxB Data register for Buff Dram.     */

        /* Sram configuration registers     */
        unsigned int  nsTimingReg;        /* Sx Timing register (for NSRam)       */
        unsigned int  wsTimingReg;        /* Sx Timing register (for WSRam)       */
        unsigned int  nsConfigReg;        /* Sx configuration register (for NSRam)*/
        unsigned int  wsConfigReg;        /* Sx configuration register (for WSRam)*/
        unsigned int  dfcdlInitConf1;     /* SR DFCDL Init Configuration1 register*/
        unsigned int  dfcdlInitConf2;     /* SR DFCDL Init Configuration2 register*/
        unsigned int  srAddressReg;       /* SR Address register.                 */
        unsigned int  srDataReg;          /* SR Data register.                    */
        unsigned int  nsSxFifoReg;       /* Sx FIFO register.  (narrow SRAM)      */
        unsigned int  wsSxFifoReg;       /* Sx FIFO register.  (wide SRAM)        */
    }extMemRegs;

    struct _debugRegs
    {
        unsigned int  controlReg;       /* Debug Control Register.                */

    }debugRegs;

    struct _trafficClass
    {
        unsigned int  tcbControl;         /* TCB Control Registers */
        unsigned int  tcbCntlExt;         /* TCB Extended Control Register */
        unsigned int  tcTableBase;        /* Traffic Conditioning Table Base Addr */
        unsigned int  pcktCntLimit;       /* TCB Packet Counter Limit */
        unsigned int  cduCntLimit;        /* TCB CDU Counter Limit */
        unsigned int  polCntLimit;        /* TCB Policing Counter Limit */
        unsigned int  countAlarm;         /* TCB Counter Alarm */
        unsigned int  polCountAlarm;      /* TCB Policing Counter Alarm */
        unsigned int  inBlockCtrl;        /* Inblock Control          */
        unsigned int  Aging0;             /* TCB aging registers      */
        unsigned int  Aging1;
        unsigned int  Aging2;
        unsigned int  keyTableAddrBase;    /*Key Table Base Address Register   */
        unsigned int  keyTableSize;        /*Key Table Size Register           */
        unsigned int  maxBucketSize;      /* Max bucket size Register.        */
        unsigned int  internalSipReg[2];  /* Internal Sip registers [0,1]     */
        unsigned int  internalDipReg[2];  /* Internal Dip registers [0,1]     */
        unsigned int  subnetMaskSizeReg;  /* Subnet mask size register.       */
        unsigned int  mcBoundaryAddrReg;  /* Multicast boundary address reg.  */
        unsigned int  mcBoundaryMaskReg;  /* Multicast boundary mask reg.     */

        struct
        {
            unsigned int greenPckt;       /* Management Green Packet Counter */
            unsigned int greenOctetLow;   /* Management Green Octet Counter Low */
            unsigned int greenOctetHigh;  /* Management Green Octet Counter High */
            unsigned int yellowPckt;      /* Management Yellow Packet Counter */
            unsigned int yellowOctetLow;  /* Management Yellow Octet Counter Low */
            unsigned int yellowOctetHigh; /* Management Yellow Octet Counter High */
            unsigned int redPckt;         /* Management Red Packet Counter */
            unsigned int redOctetLow;     /* Management Red Octet Counter Low */
            unsigned int redOctetHigh;    /* Management Red Octet Counter High */
            unsigned int discardPckt;     /* Management Discard Packet Counter */
            unsigned int discardOctetLow; /* Management Discard Octet Counter Low */
            unsigned int discardOctetHigh;/* Management Discard Octet Counter High */
        }tcCounSet[3];

        unsigned int  glRcvPckt;          /* Global Received Packets */
        unsigned int  glRcvOctet;         /* Global Received Octets */
        unsigned int  glDropPckt;         /* Global Dropped Packets */
        unsigned int  glDropOctet;        /* Global Dropped Octets */

        unsigned int  tcbShadow;          /* TCB Shadow Register */

        struct _cosTables
        {
            unsigned int  cosLookupTbl;
            unsigned int  cosCmdTbl;
            unsigned int  cosMarkTbl;
            unsigned int  cosRemarkTbl;   /* Used on devices support remrking table*/
        }cosTables;
    }trafficClass;

    struct _internalInlifTables
    {
        unsigned int  defaultInlifReg0;   /* Default Input Logical Interface Reg 0 */
        unsigned int  defaultInlifReg1;   /* Default Input Logical Interface Reg 1 */
        unsigned int  defaultInlifReg2;   /* Default Input Logical Interface Reg 2 */
        unsigned int  inlifErrAddr;       /* Input Logical Interface Erroneous Addr */
        unsigned int  portToInlifReg;     /* Port to Input Logical Interface Reg   */
        unsigned int  portToInlifHigh;    /* Port to InLif Reg for ports > 31  */
        unsigned int  swInlifEntryBookmark;
        unsigned int  swInlifEntryType;
        unsigned int  swInlifEntrySize;
        unsigned int  *inlifEntry;        /* Internal Input Logical Interface Table */
    }internalInlifTables;

    struct _sFlowRegs
    {
            unsigned int  sFlowControl;       /* sflow control */
            unsigned int  sFlowValue;         /* sflow counter start value */
            unsigned int  sFlowStatus;        /* sflow status */
            unsigned int  sFlowCounter;       /* sflow counter current value */
    }sFlowRegs;



}PP_REGS_ADDR, *PP_REGS_ADDR_PTR;


#endif /* __gtDrvPpRegsh */

