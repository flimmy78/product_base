/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvDuneUtfExtras.h
*
* DESCRIPTION:
*       Internal header which defines API for helpers functions
*       which are specific for DUNE unit testing.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvDuneUtfExtrash
#define __prvDuneUtfExtrash

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <SAND/Utils/include/sand_framework.h>


/*******************************************************************************
* prvDuneUtfExtrasInit
*
* DESCRIPTION:
*       Initialize extras for DUNE unit testing.
*       This function inits available devices and ports per device.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvDuneUtfExtrasInit
(
    void
);


/******************************************************************************\
 *                                  DUNE UT API                               *
\******************************************************************************/

/*******************************************************************************
* prvDuneUtfNextDeviceReset
*
* DESCRIPTION:
*       Reset device iterator must be called before go over all devices
*
* INPUTS:
*       devPtr   -   iterator through devices
*
* OUTPUTS:
*       None
*
* RETURNS:
*       SAND_OK  -   iterator initialization OK
*       SAND_ERR -   general failure error
*
* COMMENTS:
*       None.
*******************************************************************************/
SAND_RET prvDuneUtfNextDeviceReset
(
    SAND_INOUT   SAND_UINT   *devPtr
);

/*******************************************************************************
* prvDuneUtfNextDeviceGet
*
* DESCRIPTION:
*       This routine returns next active/non active device id.
*       Call the function until non SAND_OK error. Note that
*       iterator must be initialized before.
*
* INPUTS:
*       devPtr   -   device id
*       activeIs -   set to TRUE for getting active devices
*
* OUTPUTS:
*       None.
*
*       SAND_OK  -   Get next device was OK
*       SAND_ERR -   There is no active devices
*
* COMMENTS:
*       None.
*******************************************************************************/
SAND_RET prvDuneUtfNextDeviceGet
(
    SAND_INOUT  SAND_UINT  *devPtr,
    SAND_IN     SAND_BOOL   activeIs
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvDuneUtfExtrash */

