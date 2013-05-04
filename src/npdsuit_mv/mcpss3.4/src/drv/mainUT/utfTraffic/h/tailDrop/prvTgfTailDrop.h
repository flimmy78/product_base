/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfTailDrop.h
*
* DESCRIPTION:
*       Enhanced UTs for CPSS Tail Drop
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfTailDroph
#define __prvTgfTailDroph

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * typedef: PRV_TGF_DROP_CONFIG_SET_ENT
 *
 * Description:  There are several Drop Config Sets of parameters for the test.
 *               This enumeration specifies which Drop Config Set should be
 *               given for the test.
 */
typedef enum
{
    PRV_TGF_DROP_CONFIG_SET_0_E = 0,
    PRV_TGF_DROP_CONFIG_SET_1_E,

    PRV_TGF_DROP_CONFIG_SET_LAST_E
}PRV_TGF_DROP_CONFIG_SET_ENT;

/*******************************************************************************
* prvTgfTailDropConfigurationSet
*
* DESCRIPTION:
*       Set common configuration
*
* INPUTS:
*       isVariousPorts - GT_TRUE  creates FDB entries with different MAC DA
*                        GT_FALSE creates FDB entries with  the same MAC DA
*       numEntries     - number of QoS, FDB and PCL entries to create.
*       dropPrecedence - drop precedence for cos profile.
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
GT_VOID prvTgfTailDropConfigurationSet
(
    IN GT_BOOL           isVariousPorts,
    IN GT_U32            numEntries,
    IN CPSS_DP_LEVEL_ENT dropPrecedence
);

/*******************************************************************************
* prvTgfTailDropProfileConfigurationSet
*
* DESCRIPTION:
*       Set Drop Profile configuration
*
* INPUTS:
*       isVariousPorts - GT_TRUE  creates FDB entries with different MAC DA
*                        GT_FALSE creates FDB entries with  the same MAC DA
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvTgfTailDropProfileConfigurationSet
(
    IN GT_BOOL isVariousPorts
);

/*******************************************************************************
* prvTgfTailDropWrtdConfigurationSet
*
* DESCRIPTION:
*       Set Drop Profile configuration
*
* INPUTS:
*       configSetIdx - index of set of parameters for using in the test
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
GT_VOID prvTgfTailDropWrtdConfigurationSet
(
    IN PRV_TGF_DROP_CONFIG_SET_ENT configSetIdx
);

/*******************************************************************************
* prvTgfTailDropSharingConfigurationSet
*
* DESCRIPTION:
*       Set Drop Profile configuration
*
* INPUTS:
*       enableMode - Drop Precedence (DP) enable mode to use the shared pool
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
GT_VOID prvTgfTailDropSharingConfigurationSet
(
    IN PRV_TGF_PORT_TX_SHARED_DP_MODE_ENT enableMode
);

/*******************************************************************************
* prvTgfTailDropSharedPoolsConfigurationSet
*
* DESCRIPTION:
*       Set Drop Profile configuration
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
GT_VOID prvTgfTailDropSharedPoolsConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTailDropVariousPortsTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic
*
* INPUTS:
*       burstCount - number of packets in one burst to transmit
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
GT_VOID prvTgfTailDropVariousPortsTrafficGenerate
(
    IN GT_U32 burstCount
);

/*******************************************************************************
* prvTgfTailDropOnePortTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic
*
* INPUTS:
*       fdbIdxMin  - minimum for an iterator fdbIdx
*                    FDB index fdbIdx must be decreased from
*                    PRV_TGF_DST_COUNT_CNS down to fdbIdxMin because different
*                    tests must check different quantity of destinations
*       burstCount - number of packets in one burst to transmit
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
GT_VOID prvTgfTailDropOnePortTrafficGenerate
(
    IN GT_U32 fdbIdxMin,
    IN GT_U32 burstCount
);

/*******************************************************************************
* prvTgfTailDropSharedPoolsTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic
*
* INPUTS:
*       burstCount - number of packets in one burst to transmit
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
GT_VOID prvTgfTailDropSharedPoolsTrafficGenerate
(
    IN GT_U32 burstCount
);

/*******************************************************************************
* prvTgfTailDropWrtdTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic
*
* INPUTS:
*       isWrtdEnabled - GT_TRUE for enable WRTD Threshold
*       burstCount    - number of packets in one burst to transmit
*       numExpect     - number of expected packets on receive port
*       configSetIdx  - index of set of parameters for using in the test
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
GT_VOID prvTgfTailDropWrtdTrafficGenerate
(
    IN GT_BOOL                     isWrtdEnabled,
    IN GT_U32                      burstCount,
    IN GT_U32                      numExpect,
    IN PRV_TGF_DROP_CONFIG_SET_ENT configSetIdx
);

/*******************************************************************************
* prvTgfTailDropMcConfigurationSet
*
* DESCRIPTION:
*       Set configuration for Mcast
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
GT_VOID prvTgfTailDropMcConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTailDropMcTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic
*
* INPUTS:
*       burstCount1     - number of packets in first burst to transmit
*       burstCount2     - number of packets in second burst to transmit
*       numExpectMc     - number of expected Mc buffers
*       numExpectGlobal - number of expected global buffers
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
GT_VOID prvTgfTailDropMcTrafficGenerate
(
    IN GT_U32 burstCount1,
    IN GT_U32 burstCount2,
    IN GT_U32 numExpectMcast,
    IN GT_U32 numExpectGlobal
);

/*******************************************************************************
* prvTgfTailDropConfigurationRestore
*
* DESCRIPTION:
*       Restore configuration
*
* INPUTS:
*       numEntries - number of QoS, FDB and PCL entries to restore.
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
GT_VOID prvTgfTailDropConfigurationRestore
(
    IN GT_U32 numEntries
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfTailDroph */

