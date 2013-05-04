/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* dxFamilyBoards.c
*
* DESCRIPTION:
*       Includes board specific initialization definitions and data-structures.
*       Dx Family board types.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

/*#include <appDemo/boardConfig/appDemoBoardConfig.h>*/
#include <appDemo/boardConfig/gtBoardsConfigFuncs.h>
/*******************************************************************************
 * External variables
 ******************************************************************************/
#define EMPTY_BOARD ""
#define EMPTY_LINE  {NULL , EMPTY_BOARD ,1,{""}}
/*******************************************************************************
 * Local usage variables
 ******************************************************************************/
#ifndef IMPL_DUNE
GT_BOARD_LIST_ELEMENT   boardsList[] =
{
    {/*gtDbDx24124GBoardReg*/NULL      ,"DB-DX241-24G"     ,1,{"Rev 0.1"}},  /* 1,1,0 Salsa 24 */
    {gtRdDx246_24GBoardReg             ,"RD-DX246-24G"     ,1,{"Rev 0.1"}},  /* 2,1,0 cheetah-RD 24 */
    {/*gtEv88E6183BoardReg*/NULL       ,"RD-88E6183"       ,1,{"Rev 0.1"}},  /* 3,1,0 Sapphire */
    {/*gtEv88E6093BoardReg*/NULL       ,"DB-88E6093"       ,1,{"Rev 0.1"}},  /* 4,1,0 Ruby     */
    {/*gtRd88E609324F4GBoardReg*/NULL  ,"RD-88E6093-24F4G" ,1,{"Rev 0.1"}},  /* 5,1,0 Ruby*3+Sapphire*1 (Gemstone24)*/
    {/*gtDbDx16016GBoardReg*/NULL      ,"DB-DX160-16G"     ,1,{"Rev 0.1"}},  /* 6,1,0 Salsa 16 */
    {gtDbDx27024G3XGBoardReg           ,"DB-DX270-24G-3XG" ,1,{"Rev 0.1"}},  /* 7,1,0 Cheetah 24G --- PCI */
    {gtRdDx27048G2XGBoardReg           ,"RD-DX270-48G-2XG" ,1,{"Rev 0.1"}},  /* 8,1,0 Cheetah 48G */
    {/*gtRdDx24148GBoardReg*/NULL      ,"RD-DX241-48G"     ,1,{"Rev 0.1"}},  /* 9,1,0 Salsa 48G */
    {/*gtRd88E609348F4GBoardReg*/NULL  ,"RD-88E6093-484F4G",1,{"Rev 0.1"}},  /* 10,1,0 Ruby*6+Sapphire*1 (Gemstone48)*/
    {gtDbDx24624G3XGBoardReg           ,"DB-DX246-24G-3XG" ,1,{"Rev 0.1"}},  /* 11,1,0 Cheetah 24G -- SMI */
    {NULL                              ,"24F4G"            ,1,{"Rev 0.1"}},  /* 12,1,0 Opal*3+Jade*1 */
    {/*gtDbDx10748F2GBoardReg*/NULL    ,"DB-DX107-88E6095-48F2G",2,{"Rev 0.1",/* 13,1,0 Opal*6+Diamond(107)*1 */
                                                                    "Rev 0.2"}},
    {gtDbDx28524G4XGBoardReg           ,"DB-DX285-24G-4XG" ,4,{"Rev 0.1",    /* 14,1,0 Cheetah2 24G--PCI */
                                                               "Rev 0.2 - Vr Support",
                                                               "Rev 0.3 - FC OFF",/* 14,2,0 Cheetah2 24G--PCI */
                                                               "Rev 0.4 - Ch2 as multiplexer"}},
    {gtDbDx10610GBoardReg              ,"DB-DX106-10G"     ,1,{"Rev 0.1"}},      /* 15,1,0 Cheetah 10G */
    {/*gtDbDx10648F2GBoardReg*/NULL    ,"DB-DX106-88E6095-48F2G",1,{"Rev 0.1"}},/* 16,1,0 Opal*6+Dx106*1 */
    {gtDbDx28524G4XGBoardReg   ,"DB-DX285-48G-4XG" ,3,{"Rev 0.1",       /* 17,1,0 Cheetah2 DB--48G--PCI */
                                                        "Rev 0.2 - RD-DX285",
                                                        "Rev 0.3 - FC OFF"}}, /* 17,2,0 Cheetah2 RD--48G--PCI */
    {gtDbDx348GE4HGSBoardReg   ,"DB-DX3-GP" ,3,{"Rev 0.1",      /* 18,1,0 Cheetah3 24 4XG - PEX */
                                                "Rev 0.2 - RDE tests",
                                                "Rev 0.3 - FC OFF"}},
    {gtDbDxXcat24G4HgsBoardReg ,"xCat, xCat2",9,{"Rev 0.1",      /* 19,1,0 xCat 24 4XG - PEX */
                                          "Rev 0.2 - SDMA",
                                          "Rev 0.3 - FC OFF",
                                          "Rev 0.4 - PBR",
                                          "Rev 0.5 - P24/25 SGMII mode, p26/27 QX mode",
                                          "Rev 0.6 - PBR SDMA",
                                          "Rev 0.7 - SMI only board",
                                          "Rev 0.8 - Native MII",
                                          "Rev 0.9 - PHYMAC"}},
    {gtDbDxLion48XgBoardReg,"Lion",8,{"Rev 0.1",      /* 20,1,Lion 48XG - PEX */
                                      "Rev 0.2 - performance test configuration",
                                      "Rev 0.3",
                                      "Rev 0.4",
                                      "Rev 0.5 - RD-48", /* MTL board */
                                      "Rev 0.6 - 98DX5198 all ports 1000BaseX",
                                      "Rev 0.7 - 98DX5198 all ports SGMII 1G",
                                      "Rev 0.8 - 98CX8203 2 core {0,1}",
                                      "Rev 0.9 - RD-48 PQ3 (lion_mpc8544)" /* ext. design board */}},

    EMPTY_LINE,
    EMPTY_LINE,
    EMPTY_LINE,
    EMPTY_LINE
};
#else
GT_BOARD_LIST_ELEMENT   boardsList[] =
{
    {gtDbFx950_DX3BoardReg             ,"DB-FX950-CH3-24G"     ,1,{"Rev 0.1"}},  /* 1,1,0,0 Ocelot CH3 board */
};
#endif
const GT_U32 boardListLen = sizeof(boardsList) / sizeof(GT_BOARD_LIST_ELEMENT);

