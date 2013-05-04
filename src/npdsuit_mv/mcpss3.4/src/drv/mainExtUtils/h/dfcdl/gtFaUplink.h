/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtFaUplink.h
*
* DESCRIPTION:
*       This file includes functions for Fox uplink dfcdl table calibration.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __gtFaUplinklh
#define __gtFaUplinklh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define FA_UPLINK_DFCDL_DEBUG
#define FA_UPLINK_DFCDL_PRINT

#include <cpss/extServices/os/gtOs/gtGenTypes.h>

#define DFCDL_UPLINK_H_VERSION (1)

#define DFCDL_UPLINK_DEFAULT_NUM_OF_READS (10)

/*
 * typedef: enum GT_DFCDL_UPLINK_FUNC_CURVE_ENT
 *
 * Description:
 *  DFCDL function curve that deploys the DFCDL table
 *
 * Fields:
 *      UPLINK_DFCDL_FUNC_REG - the function will be:
 *          required_value =
 *                round (measured_value/measured_entry) * required_entry.
 *
 *      UPLINK_DFCDL_FUNC_INDENT_X - the function will be:
 *          required_value =
 *                round (measured_value/(measured_entry+X)*(required_entry+X))
 *
 * Comment:
 */
typedef enum
{
    UPLINK_DFCDL_FUNC_REG  = 0,
    UPLINK_DFCDL_FUNC_INDENT_1,
    UPLINK_DFCDL_FUNC_INDENT_2,
    UPLINK_DFCDL_FUNC_INDENT_3,
    UPLINK_DFCDL_FUNC_INDENT_4,
    UPLINK_DFCDL_FUNC_INDENT_5,
    UPLINK_DFCDL_FUNC_INDENT_6
} GT_DFCDL_UPLINK_FUNC_CURVE_ENT;


/*******************************************************************************
* diagFaUplinkDfcdlCalibrationInitDev
*
* DESCRIPTION:
*   Configure the device parameters for the callibration and
*   decide whether the device will be calibrated.
*
* INPUTS:
*       devNum         - Device number.
*       activeDramIdx  - The active DRAM DFCDL IDX that was active
*                        during the manual tuning.
*       uplinkWide     - uplink wide value as was found in the manual tuning.
*       uplinkNarrow   - uplink narrow value as was found in the manual tuning.
*       curve          - function curve (Use: UPLINK_DFCDL_FUNC_REG unless
*                        specified differently by Marvell).
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_INITIALIZED  - When board wasnt init (value 0x12)
*       GT_OUT_OF_RANGE     - When "activeDramIdx" is zero or
*                             greater than DRAM DFCDL table (value 0x3)
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS diagFaUplinkDfcdlCalibrationInitDev
(
    IN  GT_U8                          devNum,
    IN  GT_U8                          activeDramIdx,
    IN  GT_U8                          uplinkWide,
    IN  GT_U8                          uplinkNarrow,
    IN  GT_DFCDL_UPLINK_FUNC_CURVE_ENT curve
);

/*******************************************************************************
* diagDfcdlUplinkTuneFuncSingleDev
*
* DESCRIPTION:
*       This the function that actually does the uplink dfcdl tuning.
*
* INPUTS:
*       devNum     - Which device to calibrate.
*       numOfReads - How many times to read the PVT entry.
*                    Since the pointer can be jittering between two intexes,
*                    the index will be read few times (recommended 10-15) and
*                    algebraic average will be calculated.
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
*       None
*
*******************************************************************************/
GT_STATUS diagDfcdlUplinkTuneFuncSingleDev
(
    IN GT_U8  devNum,
    IN GT_U32 numOfReads
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __gtFaUplinklh */

