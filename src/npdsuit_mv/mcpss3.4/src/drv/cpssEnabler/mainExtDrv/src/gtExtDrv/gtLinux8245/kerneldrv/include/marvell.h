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
* marvell.h - Marvell Development Board BSP definition.
*
* DESCRIPTION:
*       This file contains I/O addresses and related constants. 
*
* DEPENDENCIES:
*       None.
*
*******************************************************************************/

#ifndef __INCmarvellh
#define __INCmarvellh

/* defines  */

/* System address mappings */

/* Memory segments Addresses */
/* CPU physical address of 64260's registers */
#define GT64260_INTERNAL_SPACE_DECODE       0x0068
#define GT64260_INTERNAL_SPACE_SIZE         0x10000
#define GT64260_INTERNAL_SPACE_DEFAULT_ADDR 0x14000000

#define GT64260_PCI_0_ARBITER_CNTL 0x1d00
#define GT64260_PCI_1_ARBITER_CNTL 0x1d80

#define CONFIG_GT64260_ORIG_REG_BASE 0xf1000000
#define CONFIG_GT64260_NEW_BASE 1
#define CONFIG_GT64260_NEW_REG_BASE 0xfbe00000

#define BOARD_MODEL         "EV-64260A"

/* Flash definitions */
#define FLASH_BASE         ROM_BASE_ADRS   /* Base address of Flash part    */
    #define FLASH_BUS_WIDTH     4           /* Four byte width bus          */
    #define FLASH_WIDTH_MODE    16          /* 16-bit width Flash mode      */

/* Platform Tools */
#ifdef LE /* Little Endian */
    #define MV_SHORT_SWAP(X) (X)
    #define MV_WORD_SWAP(X)  (X)
    #define MV_LONG_SWAP(X)  ((l64)(X))
#else    /* Big Endian */
    #ifndef MV_SHORT_SWAP
        #define MV_SHORT_SWAP(X) ((((X)&0xff)<<8) | (((X)&0xff00)>>8))
    #endif /* MV_SHORT_SWAP */
    #ifndef MV_WORD_SWAP
        #define MV_WORD_SWAP(X)  ((((X)&0xff)<<24) |                           \
                                  (((X)&0xff00)<<8) |                          \
                                  (((X)&0xff0000)>>8) |                        \
                                  (((X)&0xff000000)>>24))
    #endif /* MV_WORD_SWAP */
    #ifndef MV_LONG_SWAP
        #define MV_LONG_SWAP(X) ((l64) ((((X)&0xffULL)<<56) |                  \
                                        (((X)&0xff00ULL)<<40) |                \
                                        (((X)&0xff0000ULL)<<24) |              \
                                        (((X)&0xff000000ULL)<<8) |             \
                                        (((X)&0xff00000000ULL)>>8) |           \
                                        (((X)&0xff0000000000ULL)>>24) |        \
                                        (((X)&0xff000000000000ULL)>>40) |      \
                                        (((X)&0xff00000000000000ULL)>>56)))
    #endif /* MV_LONG_SWAP */
#endif /* LE */

#define SWAP_BIT_NUM(bitNum)     MV_WORD_SWAP(1 << ((bitNum) % 32))

#define BUS     NONE


/* Interrupt levels */


/* Address translation macros. These improve performace */
#define NONE_CACHEABLE(address) ((unsigned long)address)
#define VIRTUAL_TO_PHY(address) ((unsigned int)address)
#define PHY_TO_VIRTUAL(address) ((unsigned int)address)

/* Bit field definitions */

#define BIT0                            0x00000001
#define BIT1                            0x00000002
#define BIT2                            0x00000004
#define BIT3                            0x00000008
#define BIT4                            0x00000010
#define BIT5                            0x00000020
#define BIT6                            0x00000040
#define BIT7                            0x00000080
#define BIT8                            0x00000100
#define BIT9                            0x00000200
#define BIT10                           0x00000400
#define BIT11                           0x00000800
#define BIT12                           0x00001000
#define BIT13                           0x00002000
#define BIT14                           0x00004000
#define BIT15                           0x00008000
#define BIT16                           0x00010000
#define BIT17                           0x00020000
#define BIT18                           0x00040000
#define BIT19                           0x00080000
#define BIT20                           0x00100000
#define BIT21                           0x00200000
#define BIT22                           0x00400000
#define BIT23                           0x00800000
#define BIT24                           0x01000000
#define BIT25                           0x02000000
#define BIT26                           0x04000000
#define BIT27                           0x08000000
#define BIT28                           0x10000000
#define BIT29                           0x20000000
#define BIT30                           0x40000000
#define BIT31                           0x80000000


/* Handy sizes */

#define _1K                           0x00000400
#define _4K                           0x00001000
#define _8K                           0x00002000
#define _16K                          0x00004000
#define _32K                          0x00008000
#define _64K                          0x00010000
#define _128K                         0x00020000
#define _256K                         0x00040000
#define _512K                         0x00080000

#define _1M                           0x00100000
#define _2M                           0x00200000
#define _4M                           0x00400000
#define _8M                           0x00800000
#define _16M                          0x01000000
#define _32M                          0x02000000
#define _64M                          0x04000000
#define _128M                         0x08000000
#define _256M                         0x10000000
#define _512M                         0x20000000

#define _1G                           0x40000000
#define _2G                           0x80000000

#define MARVELL_VENDOR_ID 0x11AB

/*
 * Typedef: enumeration CPSS_PP_DEVICE_TYPE
 *
 * Description: Defines the different device type that may exist in system.
 *
 *      known values :
 *
 *  Twist group:
 *      98MX610B     - 48 + 4 DSSMII full feature
 *      98MX620B     - 10 * 1G full feature
 *      98MX610BS    - 48 + 4 SSMII full feature
 *      98EX110BS    - 48 + 4 SSMII no external rams
 *      98EX111BS    - 48 + 4 SSMII no ns, ws
 *      98EX112BS    - 48 + 4 SSMII no ws, no df
 *      98EX110B     - 48 + 4 DSSMII no external rams
 *      98EX111B     - 48 + 4 DSSMII no ns, ws
 *      98EX112B     - 48 + 4 DSSMII no ws, no df
 *      98EX120B     - 12G no external rams
 *      98EX120B_    - 12G no external rams (TC enabled)
 *      98EX121B     - 12G no ns, no ws
 *      98EX122B     - 12G no ws, no df
 *      98EX128B     - 12G  with extrenal memories
 *      98EX129B     - 12G  with ws, and fd
 *
 *  Twist D group:
 *      98EX100D  - 24+4
 *      98EX110D  - 48+4 SSMII
 *      98EX115D  - 48+4 SSMII with external narrow sram
 *      98EX110DS - 48+4 DSSMII
 *      98EX115DS - 48+4 DSSMII with external narrow sram
 *      98EX120D  - 12G
 *      98EX125D  - 12G with external narrow sram
 *      98EX130D  - 1 * 10G (XG)
 *      98EX135D  - 1 * 10G (XG) with external sram
 *
 *  Samba/Rumba group:
 *      98MX615/8   - 48+4 SSMII   with external narrow sram
 *      98MX625/8   - 12G          with external narrow sram
 *      98MX635/8   - 1 * 10G (XG) with external sram
 *
 *
 *  Salsa group:
 *      98DX240   - 24G  Mngment Dev
 *      98DX160   - 16G  Mngment Dev
 *      98DX120   - 12G  Mngment Dev
 *      98DX241   - 24G  NO Mngment Dev
 *      98DX161   - 16G  NO Mngment Dev
 *      98DX121   - 12G  NO Mngment Dev
 *
 *  Cheetah group:
 *     98DX270,273,272     - 24G + 3 10G                  MngmentDev  - PCI/SMI
 *     98DX260,262,263,268 - 24G + 2 10G                  MngmentDev  - PCI/SMI
 *     98DX250,246,243,248 - 24G                          MngmentDev  - PCI/SMI
 *     98DX107,106         - 10G                          MngmentDev  - SMI
 *     98DX163,166,167     - 16G                          MngmentDev  - SMI
 *     98DX803             - 3*10G                        MngmentDev  - PCI
 *     98DX249             - 24G + 1 10G + 1 5G           MngmentDev  - SMI
 *     98DX269             - 24G + 1 10G + 1 10/5G + 1 5G MngmentDev  - SMI
 *
 *  Cheetah 2 group:
 *     98DX255             - 24          MngmentDev  - PCI/SMI
 *     98DX265             - 24  + 2*10G MngmentDev  - PCI/SMI
 *     98DX275             - 24  + 3*10G MngmentDev  - PCI/SMI
 *     98DX285             - 24  + 4*10G MngmentDev  - PCI/SMI
 *
 *  Cheetah 3 group:
 *     98DX286             - 24G + 4*stack      
 *     98DX806             - 6XG + 4*stack 
 *            
 *  Tiger group:
 *     98EX116   - 48+4 SSMII with external narrow sram
 *     98EX106   - 24+4 SSMII with external narrow sram
 *     98EX108   - 24+4 SSMII with external narrow sram
 *     98EX126   - 12G with external narrow sram
 *     98EX136   - 1X10G with external narrow sram
 *
 *     98EX116DI   - 48+4
 *     98EX126DI   - 12G
 *     98EX136DI   - 1X10G
 *
 *
 */
#define    CPSS_98MX610B_CNS                  0x007011AB
#define    CPSS_98MX620B_CNS                  0x004011AB
#define    CPSS_98MX610BS_CNS                 0x006011AB
#define    CPSS_98EX110BS_CNS                 0x006711AB
#define    CPSS_98EX111BS_CNS                 0x006311AB
#define    CPSS_98EX112BS_CNS                 0x006511AB
#define    CPSS_98EX110B_CNS                  0x007711AB
#define    CPSS_98EX111B_CNS                  0x007311AB
#define    CPSS_98EX112B_CNS                  0x007511AB
#define    CPSS_98EX120B_CNS                  0x005711AB
#define    CPSS_98EX120B__CNS                 0x005611AB
#define    CPSS_98EX121B_CNS                  0x005311AB
#define    CPSS_98EX122B_CNS                  0x005511AB
#define    CPSS_98EX128B_CNS                  0x005011AB
#define    CPSS_98EX129B_CNS                  0x005211AB

#define    CPSS_98EX100D_CNS                  0x00EF11AB
#define    CPSS_98EX110D_CNS                  0x00E711AB
#define    CPSS_98EX115D_CNS                  0x00E111AB
#define    CPSS_98EX110DS_CNS                 0x00F711AB
#define    CPSS_98EX115DS_CNS                 0x00F111AB
#define    CPSS_98EX120D_CNS                  0x00D711AB
#define    CPSS_98EX125D_CNS                  0x00D111AB
#define    CPSS_98EX130D_CNS                  0x01D711AB
#define    CPSS_98EX135D_CNS                  0x01D111AB

/* samba devices */
#define    CPSS_98MX615_CNS                   0x01A011AB
#define    CPSS_98MX625_CNS                   0x018011AB
#define    CPSS_98MX635_CNS                   0x01D011AB
#define    CPSS_98MX618_CNS                   0x00A011AB
#define    CPSS_98MX628_CNS                   0x009011AB
#define    CPSS_98MX638_CNS                   0x00D011AB

/* Salsa devices  */
#define    CPSS_98DX240_CNS                 0x0D1411AB
#define    CPSS_98DX160_CNS                 0x0D1511AB
#define    CPSS_98DX120_CNS                 0x0D1611AB
#define    CPSS_98DX241_CNS                 0x0D1011AB
#define    CPSS_98DX161_CNS                 0x0D1111AB
#define    CPSS_98DX121_CNS                 0x0D1211AB

/* Sapphire device */
#define    CPSS_88E6183_CNS                   0x01A311AB

/* Ruby device */
#define    CPSS_88E6093_CNS                   0x009311AB

/* Opal device */
#define    CPSS_88E6095_CNS                   0x009511AB

/* Jade device */
#define    CPSS_88E6185_CNS                   0x01A711AB
#define    CPSS_88E6155_CNS                   0x01A511AB

/* Cheetah devices */
#define    CPSS_98DX270_CNS                   0xD80411AB
#define    CPSS_98DX260_CNS                   0xD80C11AB
#define    CPSS_98DX250_CNS                   0xD81411AB

/* Cheetah+ devices */
#define    CPSS_98DX243_CNS                   0xD2D411AB
#define    CPSS_98DX273_CNS                   0xD80011AB
#define    CPSS_98DX263_CNS                   0xD80811AB
#define    CPSS_98DX253_CNS                   0xD81011AB
#define    CPSS_98DX107_CNS                   0xD81311AB
#define    CPSS_98DX106_CNS                   0xD3D311AB
#define    CPSS_98DX133_CNS                   0xD80D11AB
#define    CPSS_98DX163_CNS                   0xD2D511AB
#define    CPSS_98DX166_CNS                   0xD81511AB
#define    CPSS_98DX167_CNS                   0xD81111AB
#define    CPSS_98DX249_CNS                   0xD82611AB
#define    CPSS_98DX269_CNS                   0xD82411AB

/* Cheetah2 devices */
#define    CPSS_98DX255_CNS                   0xD91411AB
#define    CPSS_98DX265_CNS                   0xD90C11AB
#define    CPSS_98DX275_CNS                   0xD90411AB
#define    CPSS_98DX285_CNS                   0xD91C11AB
#define    CPSS_98DX125_CNS                   0xD91511AB
#define    CPSS_98DX145_CNS                   0xD90D11AB
#define    CPSS_98DX804_CNS                   0xD91811AB

/* Cheetah3 devices */
#define    CPSS_98DX286_CNS                   0xDB0011AB
#define    CPSS_98DX806_CNS                   0xDB0111AB

/* Tiger devices */
#define    CPSS_98EX116_CNS                   0x012111AB
#define    CPSS_98EX106_CNS                   0x012A11AB
#define    CPSS_98EX108_CNS                   0x012B11AB
#define    CPSS_98EX126_CNS                   0x011111AB
#define    CPSS_98EX136_CNS                   0x015111AB

/* Tiger Drop In devices */
#define    CPSS_98EX116DI_CNS                 0x012511AB
#define    CPSS_98EX126DI_CNS                 0x011511AB
#define    CPSS_98EX136DI_CNS                 0x015511AB

/* Cheetah reduced devices */
#define    CPSS_98DX248_CNS                   0xD85411AB
#define    CPSS_98DX262_CNS                   0xD84E11AB
#define    CPSS_98DX268_CNS                   0xD84C11AB
#define    CPSS_98DX803_CNS                   0xD80611AB

/* Indication for last device in list */
#define    CPSS_DEV_END                       0x7FFFFFFF


#define  PRV_CPSS_ALL_DEVICES_MAC          \
CPSS_98MX610B_CNS       ,\
CPSS_98MX620B_CNS       ,\
CPSS_98MX610BS_CNS      ,\
CPSS_98EX110BS_CNS      ,\
CPSS_98EX111BS_CNS      ,\
CPSS_98EX112BS_CNS      ,\
CPSS_98EX110B_CNS       ,\
CPSS_98EX111B_CNS       ,\
CPSS_98EX112B_CNS       ,\
CPSS_98EX120B_CNS       ,\
CPSS_98EX120B_CNS       ,\
CPSS_98EX121B_CNS       ,\
CPSS_98EX122B_CNS       ,\
CPSS_98EX128B_CNS       ,\
CPSS_98EX129B_CNS       ,\
CPSS_98EX100D_CNS       ,\
CPSS_98EX110D_CNS       ,\
CPSS_98EX115D_CNS       ,\
CPSS_98EX110DS_CNS      ,\
CPSS_98EX115DS_CNS      ,\
CPSS_98EX120D_CNS       ,\
CPSS_98EX125D_CNS       ,\
CPSS_98EX130D_CNS       ,\
CPSS_98EX135D_CNS       ,\
CPSS_98MX615_CNS        ,\
CPSS_98MX625_CNS        ,\
CPSS_98MX635_CNS        ,\
CPSS_98MX618_CNS        ,\
CPSS_98MX628_CNS        ,\
CPSS_98MX638_CNS        ,\
CPSS_98DX240_CNS        ,\
CPSS_98DX160_CNS        ,\
CPSS_98DX120_CNS        ,\
CPSS_98DX241_CNS        ,\
CPSS_98DX161_CNS        ,\
CPSS_98DX121_CNS        ,\
CPSS_88E6183_CNS        ,\
CPSS_88E6093_CNS        ,\
CPSS_88E6095_CNS        ,\
CPSS_88E6185_CNS        ,\
CPSS_88E6155_CNS        ,\
CPSS_98DX270_CNS        ,\
CPSS_98DX260_CNS        ,\
CPSS_98DX250_CNS        ,\
CPSS_98DX243_CNS        ,\
CPSS_98DX273_CNS        ,\
CPSS_98DX263_CNS        ,\
CPSS_98DX253_CNS        ,\
CPSS_98DX107_CNS        ,\
CPSS_98DX106_CNS        ,\
CPSS_98DX133_CNS        ,\
CPSS_98DX163_CNS        ,\
CPSS_98DX166_CNS        ,\
CPSS_98DX167_CNS        ,\
CPSS_98DX249_CNS        ,\
CPSS_98DX269_CNS        ,\
CPSS_98DX255_CNS        ,\
CPSS_98DX265_CNS        ,\
CPSS_98DX275_CNS        ,\
CPSS_98DX285_CNS        ,\
CPSS_98DX125_CNS        ,\
CPSS_98DX145_CNS        ,\
CPSS_98DX804_CNS        ,\
CPSS_98EX116_CNS        ,\
CPSS_98EX106_CNS        ,\
CPSS_98EX108_CNS        ,\
CPSS_98EX126_CNS        ,\
CPSS_98EX136_CNS        ,\
CPSS_98EX116DI_CNS      ,\
CPSS_98EX126DI_CNS      ,\
CPSS_98EX136DI_CNS      ,\
CPSS_98DX248_CNS        ,\
CPSS_98DX262_CNS        ,\
CPSS_98DX268_CNS        ,\
CPSS_98DX803_CNS        ,\
CPSS_DEV_END

/********************************************/

typedef char GT_8;
typedef short GT_16;
typedef long GT_32;
typedef unsigned char GT_U8;
typedef unsigned short GT_U16;
typedef unsigned long GT_U32;
typedef void GT_VOID;
typedef enum {GT_FALSE = 0, GT_TRUE = 1} GT_BOOL;

#endif  /* __INCmarvellh */
