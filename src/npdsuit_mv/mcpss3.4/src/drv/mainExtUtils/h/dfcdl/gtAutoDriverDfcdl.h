/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtAutoDriverDfcdl.h                                                                            
*                                                                                
* DESCRIPTION:                                                                   
*       This file includes functions and structures declaration
*       of the dfcdl library.                                                                    
*       This file was automatic generated.                                       
*                                                                                
* DEPENDENCIES:                                                              
*       None.                                                                    
*                                                                                
* FILE REVISION NUMBER:                                                      
*       $Revision: 1.1.1.1 $                                                           
*                                                                                
*******************************************************************************/
#ifndef __GtAutoDriverDfcdlh 
#define __GtAutoDriverDfcdlh 

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/extServices/os/gtOs/gtGenTypes.h>

/*
* Typedef: enum ftdllTigerNSDelaySet_IN_MemSelect
*
* Description: Enumeration of parameter MemSelect of galtis
*       table / command ftdllTigerNSDelaySet
*
*/
typedef enum
{
    ftdllTigerNSDelaySet_IN_MemSelect_NS1 = 0,
    ftdllTigerNSDelaySet_IN_MemSelect_NS2 = 1
} ftdllTigerNSDelaySet_IN_MemSelect;


/*******************************************************************************
* gtFtdllTigerNSDelaySet 
*
* DESCRIPTION:
*       N. SRAM DFCDL setting command (Tiger only).
*
*   INPUTS:
*       GT_U8  devNum -
*       Device number
*
*       ftdllTigerNSDelaySet_IN_MemSelect MemSelect -
*       Memory select
*
*       GT_U32 delay0 -
*       Determines the amount of delay element 0.
*
*       GT_U32 delay1 -
*       Determines the amount of delay element 1.
*
*       GT_U32 delay2 -
*       Determines the amount of delay element 2.
*
*       GT_U32 delay3 -
*       Determines the amount of delay element 3.
*
*   OUTPUTS:
*       None.
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       G_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS gtFtdllTigerNSDelaySet 
(
    IN   GT_U8  devNum,
    IN   ftdllTigerNSDelaySet_IN_MemSelect  MemSelect,
    IN   GT_U32 delay0,
    IN   GT_U32 delay1,
    IN   GT_U32 delay2,
    IN   GT_U32 delay3
);

/*******************************************************************************
* gtFtdllTigerDqsSet 
*
* DESCRIPTION:
*       No description available.
*
*   INPUTS:
*       GT_U8  devId -
*       Device Id
*
*       GT_U32 Dqs -
*       Dqs for DB SDRAM FTDLL RAM
*
*   OUTPUTS:
*       None.
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       G_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS gtFtdllTigerDqsSet 
(
    IN   GT_U8  devId,
    IN   GT_U32 Dqs
);

/*******************************************************************************
* gtFtdllTigerClkOutSet 
*
* DESCRIPTION:
*       No description available.
*
*   INPUTS:
*       GT_U8  devId -
*       Device Id
*
*       GT_U32 ClkOut -
*       ClkOut for DB SDRAM FTDLL RAM
*
*   OUTPUTS:
*       None.
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       G_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS gtFtdllTigerClkOutSet 
(
    IN   GT_U8  devId,
    IN   GT_U32 ClkOut
);

/*******************************************************************************
* gtFtdllTigerStartBurstSet 
*
* DESCRIPTION:
*       No description available.
*
*   INPUTS:
*       GT_U8  devId -
*       Device Id
*
*       GT_U32 StartBurst -
*       StartBurst for DB SDRAM FTDLL RAM
*
*   OUTPUTS:
*       None.
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       G_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS gtFtdllTigerStartBurstSet 
(
    IN   GT_U8  devId,
    IN   GT_U32 StartBurst
);

/*******************************************************************************
* gtFtdllTigerWC2CSet 
*
* DESCRIPTION:
*       No description available.
*
*   INPUTS:
*       GT_U8  devId -
*       Device Id
*
*       GT_U32 ftdll -
*       Wide rx clk
*
*   OUTPUTS:
*       None.
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       G_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS gtFtdllTigerWC2CSet 
(
    IN   GT_U8  devId,
    IN   GT_U32 ftdll
);

/*******************************************************************************
* gtFtdllTigerNC2CSet 
*
* DESCRIPTION:
*       No description available.
*
*   INPUTS:
*       GT_U8  devId -
*       Device Id
*
*       GT_U32 ftdll -
*       Narrow rx clk
*
*   OUTPUTS:
*       None.
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       G_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS gtFtdllTigerNC2CSet 
(
    IN   GT_U8  devId,
    IN   GT_U32 ftdll
);

/*
* Typedef: enum gtFtdllTigerWC2CAutoScan_IN_trafficType
*
* Description: Enumeration of parameter trafficType of galtis
*       table / command gtFtdllTigerWC2CAutoScan
*
*/
typedef enum
{
    gtFtdllTigerWC2CAutoScan_IN_trafficType_INTERNAL = 0,
    gtFtdllTigerWC2CAutoScan_IN_trafficType_EXTERNAL = 1
} gtFtdllTigerWC2CAutoScan_IN_trafficType;


/*******************************************************************************
* gtGtFtdllTigerWC2CAutoScan 
*
* DESCRIPTION:
*       Perform a wide uplink scan to avoid system stuck during scan
*       Tiger-12G and Tiger-XG and return the min-max WC2C values
*
*   INPUTS:
*       GT_U8  devNum -
*       device number
*
*       GT_U32 ftdllWC2CStart -
*       WC2C Start FTDLL Value
*
*       GT_U32 ftdllWC2CEnd -
*       WC2C End FTDLL Value
*
*       GT_U32 waitTime -
*       Wait time to read ECC event
*
*       gtFtdllTigerWC2CAutoScan_IN_trafficType trafficType -
*       internal / external traffic
*
*   OUTPUTS:
*       GT_U32 minVal -
*       minimal WC2C value that cause event
*
*       GT_U32 maxVal -
*       maximal WC2C value that cause event
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       G_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS gtGtFtdllTigerWC2CAutoScan 
(
    IN   GT_U8  devNum,
    IN   GT_U32 ftdllWC2CStart,
    IN   GT_U32 ftdllWC2CEnd,
    IN   GT_U32 waitTime,
    IN   gtFtdllTigerWC2CAutoScan_IN_trafficType    trafficType,
    OUT  GT_U32 *minVal,
    OUT  GT_U32 *maxVal
);

/*
* Typedef: enum gtFtdllTigerNC2CAutoScan_IN_trafficType
*
* Description: Enumeration of parameter trafficType of galtis
*       table / command gtFtdllTigerNC2CAutoScan
*
*/
typedef enum
{
    gtFtdllTigerNC2CAutoScan_IN_trafficType_INTERNAL = 0,
    gtFtdllTigerNC2CAutoScan_IN_trafficType_EXTERNAL = 1
} gtFtdllTigerNC2CAutoScan_IN_trafficType;


/*******************************************************************************
* gtGtFtdllTigerNC2CAutoScan 
*
* DESCRIPTION:
*       Perform a Narrow uplink scan to avoid system stuck during scan
*       Tiger-12G and Tiger-XG and return minial and maximal values that
*       caused interrupts
*
*   INPUTS:
*       GT_U8  devNum -
*       device number
*
*       GT_U32 ftdllNC2CStart -
*       NC2C Start FTDLL Value
*
*       GT_U32 ftdllNC2CStop -
*       NC2C Stop FTDLL Value
*
*       GT_U32 waitTime -
*       Wait time to read ECC event
*
*       gtFtdllTigerNC2CAutoScan_IN_trafficType trafficType -
*       internal / external traffic
*
*   OUTPUTS:
*       GT_U32 minVal -
*       minimal NC2C value that caused  events
*
*       GT_U32 maxVal -
*       maximal NC2C value that caused  events
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       G_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS gtGtFtdllTigerNC2CAutoScan 
(
    IN   GT_U8  devNum,
    IN   GT_U32 ftdllNC2CStart,
    IN   GT_U32 ftdllNC2CStop,
    IN   GT_U32 waitTime,
    IN   gtFtdllTigerNC2CAutoScan_IN_trafficType    trafficType,
    OUT  GT_U32 *minVal,
    OUT  GT_U32 *maxVal
);

/*
* Typedef: enum ftdllTigerDbDqsScan_TAB
*
* Description: 
*   Enumeration of tabs of galtis table ftdllTigerDbDqsScan
*
*/
typedef enum
{
    ftdllTigerDbDqsScan_TAB_General = 0
} ftdllTigerDbDqsScan_TAB;


/*
* Typedef: struct ftdllTigerDbDqsScan_General_FIELDS
*
* Description:
*   Structure of fields of the galtis table ftdllTigerDbDqsScan tab General.
*
* Fields:
*   DQSValue - 
*       The DQS values scanned
*   ErrorCounter - 
*       Error counter
*/
typedef struct
{
    GT_U32  DQSValue;
    GT_U32  ErrorCounter;
}ftdllTigerDbDqsScan_General_FIELDS;

/*
* Typedef: union ftdllTigerDbDqsScan_TABS_FIELDS
*
* Description:
*   Union of the tab structures of the galtis table ftdllTigerDbDqsScan.
*
*/
typedef union
{
    ftdllTigerDbDqsScan_General_FIELDS General;
}ftdllTigerDbDqsScan_TABS_FIELDS;

/*
* Typedef: struct ftdllTigerDbDqsScan_FIELDS
*
* Description:
*   Structure of fields of the galtis table ftdllTigerDbDqsScan.
*
* Fields:
*   tableFields - union of the table fields according to the table tabs
*   tabIndex - the table tab index
*
*/
typedef struct
{
    ftdllTigerDbDqsScan_TABS_FIELDS tableFields;
    ftdllTigerDbDqsScan_TAB tabIndex; /* table tab type */

}ftdllTigerDbDqsScan_FIELDS;

/*******************************************************************************
* gtFtdllTigerDbDqsScanGetTableEntry 
*
* DESCRIPTION:
*       Get first / next entry of the ftdllTigerDbDqsScan table.
*
*   INPUTS:
*       GT_U8  devId -
*       Device Id
*
*       GT_U32 DQSStart -
*       Start value of DQS
*
*       GT_U32 DQSEnd -
*       End value of DQS
*
*       GT_U32 WaitTime -
*       Wait time in ms between cycles
*
*       ftdllTigerDbDqsScan_FIELDS  tableFields -
*           the Galtis table fields
*
*       GT_U32 entryIndex   - 
*           index of current entry refresh index.
*
*   OUTPUTS:
*       ftdllTigerDbDqsScan_FIELDS  tableFields -
*            the Galtis table output fields
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       G_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS gtFtdllTigerDbDqsScanGetTableEntry 
(
    IN   GT_U8  devId,
    IN   GT_U32 DQSStart,
    IN   GT_U32 DQSEnd,
    IN   GT_U32 WaitTime,
    INOUT ftdllTigerDbDqsScan_FIELDS    *tableFields,
    IN  GT_U32  entryIndex
);
/*
* Typedef: enum ftdllTigerDbClkOutScan_TAB
*
* Description: 
*   Enumeration of tabs of galtis table ftdllTigerDbClkOutScan
*
*/
typedef enum
{
    ftdllTigerDbClkOutScan_TAB_General = 0
} ftdllTigerDbClkOutScan_TAB;


/*
* Typedef: struct ftdllTigerDbClkOutScan_General_FIELDS
*
* Description:
*   Structure of fields of the galtis table ftdllTigerDbClkOutScan tab General.
*
* Fields:
*   ClkOutValue - 
*       The ClkOut values scanned
*   ErrorCounter - 
*       Error counter
*/
typedef struct
{
    GT_U32  ClkOutValue;
    GT_U32  ErrorCounter;
}ftdllTigerDbClkOutScan_General_FIELDS;

/*
* Typedef: union ftdllTigerDbClkOutScan_TABS_FIELDS
*
* Description:
*   Union of the tab structures of the galtis table ftdllTigerDbClkOutScan.
*
*/
typedef union
{
    ftdllTigerDbClkOutScan_General_FIELDS General;
}ftdllTigerDbClkOutScan_TABS_FIELDS;

/*
* Typedef: struct ftdllTigerDbClkOutScan_FIELDS
*
* Description:
*   Structure of fields of the galtis table ftdllTigerDbClkOutScan.
*
* Fields:
*   tableFields - union of the table fields according to the table tabs
*   tabIndex - the table tab index
*
*/
typedef struct
{
    ftdllTigerDbClkOutScan_TABS_FIELDS tableFields;
    ftdllTigerDbClkOutScan_TAB tabIndex; /* table tab type */

}ftdllTigerDbClkOutScan_FIELDS;

/*******************************************************************************
* gtFtdllTigerDbClkOutScanGetTableEntry 
*
* DESCRIPTION:
*       Get first / next entry of the ftdllTigerDbClkOutScan table.
*
*   INPUTS:
*       GT_U8  devId -
*       Device Id
*
*       GT_U32 ClkOutStart -
*       Start value of clock out
*
*       GT_U32 ClkOutEnd -
*       End value of clock out
*
*       GT_U32 WaitTime -
*       Wait time in ms between cycles
*
*       ftdllTigerDbClkOutScan_FIELDS   tableFields -
*           the Galtis table fields
*
*       GT_U32 entryIndex   - 
*           index of current entry refresh index.
*
*   OUTPUTS:
*       ftdllTigerDbClkOutScan_FIELDS   tableFields -
*            the Galtis table output fields
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       G_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS gtFtdllTigerDbClkOutScanGetTableEntry 
(
    IN   GT_U8  devId,
    IN   GT_U32 ClkOutStart,
    IN   GT_U32 ClkOutEnd,
    IN   GT_U32 WaitTime,
    INOUT ftdllTigerDbClkOutScan_FIELDS *tableFields,
    IN  GT_U32  entryIndex
);
/*
* Typedef: enum ftdllTigerDbStartBurstScan_TAB
*
* Description: 
*   Enumeration of tabs of galtis table ftdllTigerDbStartBurstScan
*
*/
typedef enum
{
    ftdllTigerDbStartBurstScan_TAB_General = 0
} ftdllTigerDbStartBurstScan_TAB;


/*
* Typedef: struct ftdllTigerDbStartBurstScan_General_FIELDS
*
* Description:
*   Structure of fields of the galtis table ftdllTigerDbStartBurstScan tab General.
*
* Fields:
*   StartBurstValue - 
*       The StartBurst_out values scanned
*   ErrorCounter - 
*       Error Counter
*/
typedef struct
{
    GT_U32  StartBurstValue;
    GT_U32  ErrorCounter;
}ftdllTigerDbStartBurstScan_General_FIELDS;

/*
* Typedef: union ftdllTigerDbStartBurstScan_TABS_FIELDS
*
* Description:
*   Union of the tab structures of the galtis table ftdllTigerDbStartBurstScan.
*
*/
typedef union
{
    ftdllTigerDbStartBurstScan_General_FIELDS General;
}ftdllTigerDbStartBurstScan_TABS_FIELDS;

/*
* Typedef: struct ftdllTigerDbStartBurstScan_FIELDS
*
* Description:
*   Structure of fields of the galtis table ftdllTigerDbStartBurstScan.
*
* Fields:
*   tableFields - union of the table fields according to the table tabs
*   tabIndex - the table tab index
*
*/
typedef struct
{
    ftdllTigerDbStartBurstScan_TABS_FIELDS tableFields;
    ftdllTigerDbStartBurstScan_TAB tabIndex; /* table tab type */

}ftdllTigerDbStartBurstScan_FIELDS;

/*******************************************************************************
* gtFtdllTigerDbStartBurstScanGetTableEntry 
*
* DESCRIPTION:
*       Get first / next entry of the ftdllTigerDbStartBurstScan table.
*
*   INPUTS:
*       GT_U8  devNum -
*       Dev Num
*
*       GT_U32 StartBurstStart -
*       Start value of StartBurst_out
*
*       GT_U32 StartBurstEnd -
*       End value of StartBurst_out
*
*       GT_U32 WaitTime -
*       Wait time in ms between cycles
*
*       ftdllTigerDbStartBurstScan_FIELDS   tableFields -
*           the Galtis table fields
*
*       GT_U32 entryIndex   - 
*           index of current entry refresh index.
*
*   OUTPUTS:
*       ftdllTigerDbStartBurstScan_FIELDS   tableFields -
*            the Galtis table output fields
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       G_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS gtFtdllTigerDbStartBurstScanGetTableEntry 
(
    IN   GT_U8  devNum,
    IN   GT_U32 StartBurstStart,
    IN   GT_U32 StartBurstEnd,
    IN   GT_U32 WaitTime,
    INOUT ftdllTigerDbStartBurstScan_FIELDS *tableFields,
    IN  GT_U32  entryIndex
);
/*
* Typedef: enum ftdllTigerWC2cScan_General_UpDown
*
* Description: 
*   Enumeration of tabs of galtis table ftdllTigerDbStartBurstScan
*
*/
typedef enum
{
    ftdllTigerWC2cScan_General_UpDown_Down = 0,
    ftdllTigerWC2cScan_General_UpDown_Up = 1
} ftdllTigerWC2cScan_General_UpDown;


/*
* Typedef: enum ftdllTigerWC2cScan_TAB
*
* Description: 
*   Enumeration of tabs of galtis table ftdllTigerWC2cScan
*
*/
typedef enum
{
    ftdllTigerWC2cScan_TAB_General = 0
} ftdllTigerWC2cScan_TAB;


/*
* Typedef: struct ftdllTigerWC2cScan_General_FIELDS
*
* Description:
*   Structure of fields of the galtis table ftdllTigerWC2cScan tab General.
*
* Fields:
*   DFCDLValue - 
*       The DFCDL values scanned
*   UpDown - 
*       The result of the verify function
*   NumOfECC - 
*       Number of ECCCount
*/
typedef struct
{
    GT_U32  DFCDLValue;
    ftdllTigerWC2cScan_General_UpDown   UpDown;
    GT_U32  NumOfECC;
}ftdllTigerWC2cScan_General_FIELDS;

/*
* Typedef: union ftdllTigerWC2cScan_TABS_FIELDS
*
* Description:
*   Union of the tab structures of the galtis table ftdllTigerWC2cScan.
*
*/
typedef union
{
    ftdllTigerWC2cScan_General_FIELDS General;
}ftdllTigerWC2cScan_TABS_FIELDS;

/*
* Typedef: struct ftdllTigerWC2cScan_FIELDS
*
* Description:
*   Structure of fields of the galtis table ftdllTigerWC2cScan.
*
* Fields:
*   tableFields - union of the table fields according to the table tabs
*   tabIndex - the table tab index
*
*/
typedef struct
{
    ftdllTigerWC2cScan_TABS_FIELDS tableFields;
    ftdllTigerWC2cScan_TAB tabIndex; /* table tab type */

}ftdllTigerWC2cScan_FIELDS;

/*******************************************************************************
* gtFtdllTigerWC2cScanGetTableEntry 
*
* DESCRIPTION:
*       Get first / next entry of the ftdllTigerWC2cScan table.
*
*   INPUTS:
*       GT_U8  devNum -
*       Dev Num
*
*       GT_U32 DFCDLStart -
*       DFCDL Start
*
*       GT_U32 DFCDLEnd -
*       DFCDL End
*
*       GT_U32 nDfcdl -
*       Narrow DFCDL value
*
*       GT_U32 WaitTime -
*       Wait time defined before checking interrupt count
*
*       ftdllTigerWC2cScan_FIELDS   tableFields -
*           the Galtis table fields
*
*       GT_U32 entryIndex   - 
*           index of current entry refresh index.
*
*   OUTPUTS:
*       ftdllTigerWC2cScan_FIELDS   tableFields -
*            the Galtis table output fields
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       G_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS gtFtdllTigerWC2cScanGetTableEntry 
(
    IN   GT_U8  devNum,
    IN   GT_U32 DFCDLStart,
    IN   GT_U32 DFCDLEnd,
    IN   GT_U32 nDfcdl,
    IN   GT_U32 WaitTime,
    INOUT ftdllTigerWC2cScan_FIELDS *tableFields,
    IN  GT_U32  entryIndex
);
/*
* Typedef: enum ftdllTigerNC2cScan_General_UpDown
*
* Description: 
*   Enumeration of tabs of galtis table ftdllTigerWC2cScan
*
*/
typedef enum
{
    ftdllTigerNC2cScan_General_UpDown_Down = 0,
    ftdllTigerNC2cScan_General_UpDown_Up = 1
} ftdllTigerNC2cScan_General_UpDown;


/*
* Typedef: enum ftdllTigerNC2cScan_TAB
*
* Description: 
*   Enumeration of tabs of galtis table ftdllTigerNC2cScan
*
*/
typedef enum
{
    ftdllTigerNC2cScan_TAB_General = 0
} ftdllTigerNC2cScan_TAB;


/*
* Typedef: struct ftdllTigerNC2cScan_General_FIELDS
*
* Description:
*   Structure of fields of the galtis table ftdllTigerNC2cScan tab General.
*
* Fields:
*   DFCDLValue - 
*       The DFCDL values scanned
*   UpDown - 
*       The result of the verify function
*   NumOfECC - 
*       Number of ECCCount
*/
typedef struct
{
    GT_U32  DFCDLValue;
    ftdllTigerNC2cScan_General_UpDown   UpDown;
    GT_U32  NumOfECC;
}ftdllTigerNC2cScan_General_FIELDS;

/*
* Typedef: union ftdllTigerNC2cScan_TABS_FIELDS
*
* Description:
*   Union of the tab structures of the galtis table ftdllTigerNC2cScan.
*
*/
typedef union
{
    ftdllTigerNC2cScan_General_FIELDS General;
}ftdllTigerNC2cScan_TABS_FIELDS;

/*
* Typedef: struct ftdllTigerNC2cScan_FIELDS
*
* Description:
*   Structure of fields of the galtis table ftdllTigerNC2cScan.
*
* Fields:
*   tableFields - union of the table fields according to the table tabs
*   tabIndex - the table tab index
*
*/
typedef struct
{
    ftdllTigerNC2cScan_TABS_FIELDS tableFields;
    ftdllTigerNC2cScan_TAB tabIndex; /* table tab type */

}ftdllTigerNC2cScan_FIELDS;

/*******************************************************************************
* gtFtdllTigerNC2cScanGetTableEntry 
*
* DESCRIPTION:
*       Get first / next entry of the ftdllTigerNC2cScan table.
*
*   INPUTS:
*       GT_U8  devNum -
*       Dev Num
*
*       GT_U32 DFCDLStart -
*       DFCDL Start
*
*       GT_U32 DFCDLEnd -
*       DFCDL End
*
*       GT_U32 wDfcdl -
*       Wide DFCDL value
*
*       GT_U32 WaitTime -
*       Wait time defined before checking interrupt count
*
*       ftdllTigerNC2cScan_FIELDS   tableFields -
*           the Galtis table fields
*
*       GT_U32 entryIndex   - 
*           index of current entry refresh index.
*
*   OUTPUTS:
*       ftdllTigerNC2cScan_FIELDS   tableFields -
*            the Galtis table output fields
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       G_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS gtFtdllTigerNC2cScanGetTableEntry 
(
    IN   GT_U8  devNum,
    IN   GT_U32 DFCDLStart,
    IN   GT_U32 DFCDLEnd,
    IN   GT_U32 wDfcdl,
    IN   GT_U32 WaitTime,
    INOUT ftdllTigerNC2cScan_FIELDS *tableFields,
    IN  GT_U32  entryIndex
);
/*
* Typedef: enum ftdllTigerNSClkOutScan_IN_MemSelect
*
* Description: 
*   Enumeration of tabs of galtis table ftdllTigerNC2cScan
*
*/
typedef enum
{
    ftdllTigerNSClkOutScan_IN_MemSelect_NS1 = 0,
    ftdllTigerNSClkOutScan_IN_MemSelect_NS2 = 1
} ftdllTigerNSClkOutScan_IN_MemSelect;


/*
* Typedef: enum ftdllTigerNSClkOutScan_TAB
*
* Description: 
*   Enumeration of tabs of galtis table ftdllTigerNSClkOutScan
*
*/
typedef enum
{
    ftdllTigerNSClkOutScan_TAB_General = 0
} ftdllTigerNSClkOutScan_TAB;


/*
* Typedef: struct ftdllTigerNSClkOutScan_General_FIELDS
*
* Description:
*   Structure of fields of the galtis table ftdllTigerNSClkOutScan tab General.
*
* Fields:
*   DFCDLValue - 
*       The DFCDL values scanned
*   ErrorCounter - 
*       Error counter
*/
typedef struct
{
    GT_U32  DFCDLValue;
    GT_U32  ErrorCounter;
}ftdllTigerNSClkOutScan_General_FIELDS;

/*
* Typedef: union ftdllTigerNSClkOutScan_TABS_FIELDS
*
* Description:
*   Union of the tab structures of the galtis table ftdllTigerNSClkOutScan.
*
*/
typedef union
{
    ftdllTigerNSClkOutScan_General_FIELDS General;
}ftdllTigerNSClkOutScan_TABS_FIELDS;

/*
* Typedef: struct ftdllTigerNSClkOutScan_FIELDS
*
* Description:
*   Structure of fields of the galtis table ftdllTigerNSClkOutScan.
*
* Fields:
*   tableFields - union of the table fields according to the table tabs
*   tabIndex - the table tab index
*
*/
typedef struct
{
    ftdllTigerNSClkOutScan_TABS_FIELDS tableFields;
    ftdllTigerNSClkOutScan_TAB tabIndex; /* table tab type */

}ftdllTigerNSClkOutScan_FIELDS;

/*******************************************************************************
* gtFtdllTigerNSClkOutScanGetTableEntry 
*
* DESCRIPTION:
*       Get first / next entry of the ftdllTigerNSClkOutScan table.
*
*   INPUTS:
*       GT_U8  devNum -
*       Dev Num
*
*       ftdllTigerNSClkOutScan_IN_MemSelect MemSelect -
*       Memory select
*
*       GT_U32 ClkOutStart -
*       DFCDL Start
*
*       GT_U32 ClkOutEnd -
*       DFCDL End
*
*       GT_U32 WaitTime -
*       Wait time in ms
*
*       ftdllTigerNSClkOutScan_FIELDS   tableFields -
*           the Galtis table fields
*
*       GT_U32 entryIndex   - 
*           index of current entry refresh index.
*
*   OUTPUTS:
*       ftdllTigerNSClkOutScan_FIELDS   tableFields -
*            the Galtis table output fields
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       G_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS gtFtdllTigerNSClkOutScanGetTableEntry 
(
    IN   GT_U8  devNum,
    IN   ftdllTigerNSClkOutScan_IN_MemSelect    MemSelect,
    IN   GT_U32 ClkOutStart,
    IN   GT_U32 ClkOutEnd,
    IN   GT_U32 WaitTime,
    INOUT ftdllTigerNSClkOutScan_FIELDS *tableFields,
    IN  GT_U32  entryIndex
);
/*
* Typedef: enum ftdllTigerNSRetClkScan_IN_MemSelect
*
* Description: 
*   Enumeration of tabs of galtis table ftdllTigerNSClkOutScan
*
*/
typedef enum
{
    ftdllTigerNSRetClkScan_IN_MemSelect_NS1 = 0,
    ftdllTigerNSRetClkScan_IN_MemSelect_NS2 = 1
} ftdllTigerNSRetClkScan_IN_MemSelect;


/*
* Typedef: enum ftdllTigerNSRetClkScan_TAB
*
* Description: 
*   Enumeration of tabs of galtis table ftdllTigerNSRetClkScan
*
*/
typedef enum
{
    ftdllTigerNSRetClkScan_TAB_General = 0
} ftdllTigerNSRetClkScan_TAB;


/*
* Typedef: struct ftdllTigerNSRetClkScan_General_FIELDS
*
* Description:
*   Structure of fields of the galtis table ftdllTigerNSRetClkScan tab General.
*
* Fields:
*   DFCDLValue - 
*       Current DFCDL value
*   ErrorCounter - 
*       Error counter
*/
typedef struct
{
    GT_U32  DFCDLValue;
    GT_U32  ErrorCounter;
}ftdllTigerNSRetClkScan_General_FIELDS;

/*
* Typedef: union ftdllTigerNSRetClkScan_TABS_FIELDS
*
* Description:
*   Union of the tab structures of the galtis table ftdllTigerNSRetClkScan.
*
*/
typedef union
{
    ftdllTigerNSRetClkScan_General_FIELDS General;
}ftdllTigerNSRetClkScan_TABS_FIELDS;

/*
* Typedef: struct ftdllTigerNSRetClkScan_FIELDS
*
* Description:
*   Structure of fields of the galtis table ftdllTigerNSRetClkScan.
*
* Fields:
*   tableFields - union of the table fields according to the table tabs
*   tabIndex - the table tab index
*
*/
typedef struct
{
    ftdllTigerNSRetClkScan_TABS_FIELDS tableFields;
    ftdllTigerNSRetClkScan_TAB tabIndex; /* table tab type */

}ftdllTigerNSRetClkScan_FIELDS;

/*******************************************************************************
* gtFtdllTigerNSRetClkScanGetTableEntry 
*
* DESCRIPTION:
*       Get first / next entry of the ftdllTigerNSRetClkScan table.
*
*   INPUTS:
*       GT_U8  devId -
*       Device Id
*
*       ftdllTigerNSRetClkScan_IN_MemSelect MemSelect -
*       Memory select
*
*       GT_U32 RetClkStart -
*       No description available.
*
*       GT_U32 RetClkEnd -
*       No description available.
*
*       GT_U32 WaitTime -
*       Wait time in ms
*
*       ftdllTigerNSRetClkScan_FIELDS   tableFields -
*           the Galtis table fields
*
*       GT_U32 entryIndex   - 
*           index of current entry refresh index.
*
*   OUTPUTS:
*       ftdllTigerNSRetClkScan_FIELDS   tableFields -
*            the Galtis table output fields
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       G_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS gtFtdllTigerNSRetClkScanGetTableEntry 
(
    IN   GT_U8  devId,
    IN   ftdllTigerNSRetClkScan_IN_MemSelect    MemSelect,
    IN   GT_U32 RetClkStart,
    IN   GT_U32 RetClkEnd,
    IN   GT_U32 WaitTime,
    INOUT ftdllTigerNSRetClkScan_FIELDS *tableFields,
    IN  GT_U32  entryIndex
);
/*
* Typedef: enum ftdllTigerNSRetClkInvScan_IN_MemSelect
*
* Description: 
*   Enumeration of tabs of galtis table ftdllTigerNSRetClkScan
*
*/
typedef enum
{
    ftdllTigerNSRetClkInvScan_IN_MemSelect_NS1 = 0,
    ftdllTigerNSRetClkInvScan_IN_MemSelect_NS2 = 1
} ftdllTigerNSRetClkInvScan_IN_MemSelect;


/*
* Typedef: enum ftdllTigerNSRetClkInvScan_TAB
*
* Description: 
*   Enumeration of tabs of galtis table ftdllTigerNSRetClkInvScan
*
*/
typedef enum
{
    ftdllTigerNSRetClkInvScan_TAB_General = 0
} ftdllTigerNSRetClkInvScan_TAB;


/*
* Typedef: struct ftdllTigerNSRetClkInvScan_General_FIELDS
*
* Description:
*   Structure of fields of the galtis table ftdllTigerNSRetClkInvScan tab General.
*
* Fields:
*   DFCDLValue - 
*       No description available.
*   ErrorCounter - 
*       Error counter
*/
typedef struct
{
    GT_U32  DFCDLValue;
    GT_U32  ErrorCounter;
}ftdllTigerNSRetClkInvScan_General_FIELDS;

/*
* Typedef: union ftdllTigerNSRetClkInvScan_TABS_FIELDS
*
* Description:
*   Union of the tab structures of the galtis table ftdllTigerNSRetClkInvScan.
*
*/
typedef union
{
    ftdllTigerNSRetClkInvScan_General_FIELDS General;
}ftdllTigerNSRetClkInvScan_TABS_FIELDS;

/*
* Typedef: struct ftdllTigerNSRetClkInvScan_FIELDS
*
* Description:
*   Structure of fields of the galtis table ftdllTigerNSRetClkInvScan.
*
* Fields:
*   tableFields - union of the table fields according to the table tabs
*   tabIndex - the table tab index
*
*/
typedef struct
{
    ftdllTigerNSRetClkInvScan_TABS_FIELDS tableFields;
    ftdllTigerNSRetClkInvScan_TAB tabIndex; /* table tab type */

}ftdllTigerNSRetClkInvScan_FIELDS;

/*******************************************************************************
* gtFtdllTigerNSRetClkInvScanGetTableEntry 
*
* DESCRIPTION:
*       Get first / next entry of the ftdllTigerNSRetClkInvScan table.
*
*   INPUTS:
*       GT_U8  devId -
*       Device Id
*
*       ftdllTigerNSRetClkInvScan_IN_MemSelect MemSelect -
*       Memory select
*
*       GT_U32 RetClkInvStart -
*       No description available.
*
*       GT_U32 RetClkInvEnd -
*       No description available.
*
*       GT_U32 WaitTime -
*       Wait time in ms
*
*       ftdllTigerNSRetClkInvScan_FIELDS    tableFields -
*           the Galtis table fields
*
*       GT_U32 entryIndex   - 
*           index of current entry refresh index.
*
*   OUTPUTS:
*       ftdllTigerNSRetClkInvScan_FIELDS    tableFields -
*            the Galtis table output fields
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       G_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS gtFtdllTigerNSRetClkInvScanGetTableEntry 
(
    IN   GT_U8  devId,
    IN   ftdllTigerNSRetClkInvScan_IN_MemSelect MemSelect,
    IN   GT_U32 RetClkInvStart,
    IN   GT_U32 RetClkInvEnd,
    IN   GT_U32 WaitTime,
    INOUT ftdllTigerNSRetClkInvScan_FIELDS  *tableFields,
    IN  GT_U32  entryIndex
);
/*
* Typedef: enum dfcdlAutoScanDqs_IN_trafficType
*
* Description: 
*   Enumeration of tabs of galtis table ftdllTigerNSRetClkInvScan
*
*/
typedef enum
{
    dfcdlAutoScanDqs_IN_trafficType_INTERNAL = 0,
    dfcdlAutoScanDqs_IN_trafficType_EXTERNAL = 1
} dfcdlAutoScanDqs_IN_trafficType;


/*
* Typedef: enum dfcdlAutoScanDqs_TAB
*
* Description: 
*   Enumeration of tabs of galtis table dfcdlAutoScanDqs
*
*/
typedef enum
{
    dfcdlAutoScanDqs_TAB_General = 0
} dfcdlAutoScanDqs_TAB;


/*
* Typedef: struct dfcdlAutoScanDqs_General_FIELDS
*
* Description:
*   Structure of fields of the galtis table dfcdlAutoScanDqs tab General.
*
* Fields:
*   Dqs - 
*       current Dqs value
*   errorCounterLow - 
*       MIB error counter - LSB
*   errorCounterHigh - 
*       MIB error counter - MSB
*/
typedef struct
{
    GT_U32  Dqs;
    GT_U32  errorCounterLow;
    GT_U32  errorCounterHigh;
}dfcdlAutoScanDqs_General_FIELDS;

/*
* Typedef: union dfcdlAutoScanDqs_TABS_FIELDS
*
* Description:
*   Union of the tab structures of the galtis table dfcdlAutoScanDqs.
*
*/
typedef union
{
    dfcdlAutoScanDqs_General_FIELDS General;
}dfcdlAutoScanDqs_TABS_FIELDS;

/*
* Typedef: struct dfcdlAutoScanDqs_FIELDS
*
* Description:
*   Structure of fields of the galtis table dfcdlAutoScanDqs.
*
* Fields:
*   tableFields - union of the table fields according to the table tabs
*   tabIndex - the table tab index
*
*/
typedef struct
{
    dfcdlAutoScanDqs_TABS_FIELDS tableFields;
    dfcdlAutoScanDqs_TAB tabIndex; /* table tab type */

}dfcdlAutoScanDqs_FIELDS;

/*******************************************************************************
* gtDfcdlAutoScanDqsGetTableEntry 
*
* DESCRIPTION:
*       Get first / next entry of the dfcdlAutoScanDqs table.
*
*   INPUTS:
*       GT_U8  devNum -
*       device number
*
*       GT_U32 dqsStart -
*       dqs first value
*
*       GT_U32 dqsEnd -
*       dqs last value
*
*       GT_U32 waitTime -
*       time to wait before read MIB counters
*
*       dfcdlAutoScanDqs_IN_trafficType trafficType -
*       internal / external traffic
*
*       dfcdlAutoScanDqs_FIELDS tableFields -
*           the Galtis table fields
*
*       GT_U32 entryIndex   - 
*           index of current entry refresh index.
*
*   OUTPUTS:
*       dfcdlAutoScanDqs_FIELDS tableFields -
*            the Galtis table output fields
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       G_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS gtDfcdlAutoScanDqsGetTableEntry 
(
    IN   GT_U8  devNum,
    IN   GT_U32 dqsStart,
    IN   GT_U32 dqsEnd,
    IN   GT_U32 waitTime,
    IN   dfcdlAutoScanDqs_IN_trafficType    trafficType,
    INOUT dfcdlAutoScanDqs_FIELDS   *tableFields,
    IN  GT_U32  entryIndex
);
/*
* Typedef: enum dfcdlAutoScanClkOut_IN_trafficType
*
* Description: 
*   Enumeration of tabs of galtis table dfcdlAutoScanDqs
*
*/
typedef enum
{
    dfcdlAutoScanClkOut_IN_trafficType_INTERNAL = 0,
    dfcdlAutoScanClkOut_IN_trafficType_EXTERNAL = 1
} dfcdlAutoScanClkOut_IN_trafficType;


/*
* Typedef: enum dfcdlAutoScanClkOut_TAB
*
* Description: 
*   Enumeration of tabs of galtis table dfcdlAutoScanClkOut
*
*/
typedef enum
{
    dfcdlAutoScanClkOut_TAB_General = 0
} dfcdlAutoScanClkOut_TAB;


/*
* Typedef: struct dfcdlAutoScanClkOut_General_FIELDS
*
* Description:
*   Structure of fields of the galtis table dfcdlAutoScanClkOut tab General.
*
* Fields:
*   clkOut - 
*       current clkOut value
*   errorCounterLow - 
*       MIB error counter LSB
*   errorCounterHigh - 
*       MIB error counter MSB
*/
typedef struct
{
    GT_U32  clkOut;
    GT_U32  errorCounterLow;
    GT_U32  errorCounterHigh;
}dfcdlAutoScanClkOut_General_FIELDS;

/*
* Typedef: union dfcdlAutoScanClkOut_TABS_FIELDS
*
* Description:
*   Union of the tab structures of the galtis table dfcdlAutoScanClkOut.
*
*/
typedef union
{
    dfcdlAutoScanClkOut_General_FIELDS General;
}dfcdlAutoScanClkOut_TABS_FIELDS;

/*
* Typedef: struct dfcdlAutoScanClkOut_FIELDS
*
* Description:
*   Structure of fields of the galtis table dfcdlAutoScanClkOut.
*
* Fields:
*   tableFields - union of the table fields according to the table tabs
*   tabIndex - the table tab index
*
*/
typedef struct
{
    dfcdlAutoScanClkOut_TABS_FIELDS tableFields;
    dfcdlAutoScanClkOut_TAB tabIndex; /* table tab type */

}dfcdlAutoScanClkOut_FIELDS;

/*******************************************************************************
* gtDfcdlAutoScanClkOutGetTableEntry 
*
* DESCRIPTION:
*       Get first / next entry of the dfcdlAutoScanClkOut table.
*
*   INPUTS:
*       GT_U8  devNum -
*       device number
*
*       GT_U32 clkOutStart -
*       first clkOut value
*
*       GT_U32 clkOutLast -
*       last clkOut value
*
*       GT_U32 waitTime -
*       time to wait to CRC error
*
*       dfcdlAutoScanClkOut_IN_trafficType trafficType -
*       internal / external traffic
*
*       dfcdlAutoScanClkOut_FIELDS  tableFields -
*           the Galtis table fields
*
*       GT_U32 entryIndex   - 
*           index of current entry refresh index.
*
*   OUTPUTS:
*       dfcdlAutoScanClkOut_FIELDS  tableFields -
*            the Galtis table output fields
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       G_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS gtDfcdlAutoScanClkOutGetTableEntry 
(
    IN   GT_U8  devNum,
    IN   GT_U32 clkOutStart,
    IN   GT_U32 clkOutLast,
    IN   GT_U32 waitTime,
    IN   dfcdlAutoScanClkOut_IN_trafficType trafficType,
    INOUT dfcdlAutoScanClkOut_FIELDS    *tableFields,
    IN  GT_U32  entryIndex
);
/*
* Typedef: enum dfcdlAutoScanStartBurst_IN_traffictype
*
* Description: 
*   Enumeration of tabs of galtis table dfcdlAutoScanClkOut
*
*/
typedef enum
{
    dfcdlAutoScanStartBurst_IN_traffictype_INTERNAL = 0,
    dfcdlAutoScanStartBurst_IN_traffictype_EXTERNAL = 1
} dfcdlAutoScanStartBurst_IN_traffictype;


/*
* Typedef: enum dfcdlAutoScanStartBurst_TAB
*
* Description: 
*   Enumeration of tabs of galtis table dfcdlAutoScanStartBurst
*
*/
typedef enum
{
    dfcdlAutoScanStartBurst_TAB_General = 0
} dfcdlAutoScanStartBurst_TAB;


/*
* Typedef: struct dfcdlAutoScanStartBurst_General_FIELDS
*
* Description:
*   Structure of fields of the galtis table dfcdlAutoScanStartBurst tab General.
*
* Fields:
*   startBurst - 
*       current startBurst value
*   errorCounterLow - 
*       MIB error counter LSB
*   errorCounterHigh - 
*       MIB error counter MSB
*/
typedef struct
{
    GT_U32  startBurst;
    GT_U32  errorCounterLow;
    GT_U32  errorCounterHigh;
}dfcdlAutoScanStartBurst_General_FIELDS;

/*
* Typedef: union dfcdlAutoScanStartBurst_TABS_FIELDS
*
* Description:
*   Union of the tab structures of the galtis table dfcdlAutoScanStartBurst.
*
*/
typedef union
{
    dfcdlAutoScanStartBurst_General_FIELDS General;
}dfcdlAutoScanStartBurst_TABS_FIELDS;

/*
* Typedef: struct dfcdlAutoScanStartBurst_FIELDS
*
* Description:
*   Structure of fields of the galtis table dfcdlAutoScanStartBurst.
*
* Fields:
*   tableFields - union of the table fields according to the table tabs
*   tabIndex - the table tab index
*
*/
typedef struct
{
    dfcdlAutoScanStartBurst_TABS_FIELDS tableFields;
    dfcdlAutoScanStartBurst_TAB tabIndex; /* table tab type */

}dfcdlAutoScanStartBurst_FIELDS;

/*******************************************************************************
* gtDfcdlAutoScanStartBurstGetTableEntry 
*
* DESCRIPTION:
*       Get first / next entry of the dfcdlAutoScanStartBurst table.
*
*   INPUTS:
*       GT_U8  devNum -
*       device number
*
*       GT_U32 startBurstStart -
*       first startBurst value
*
*       GT_U32 lastBurstStart -
*       last startBurst value
*
*       GT_U32 waitTime -
*       wait time
*
*       dfcdlAutoScanStartBurst_IN_traffictype traffictype -
*       internal / external trrafic
*
*       dfcdlAutoScanStartBurst_FIELDS  tableFields -
*           the Galtis table fields
*
*       GT_U32 entryIndex   - 
*           index of current entry refresh index.
*
*   OUTPUTS:
*       dfcdlAutoScanStartBurst_FIELDS  tableFields -
*            the Galtis table output fields
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       G_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS gtDfcdlAutoScanStartBurstGetTableEntry 
(
    IN   GT_U8  devNum,
    IN   GT_U32 startBurstStart,
    IN   GT_U32 lastBurstStart,
    IN   GT_U32 waitTime,
    IN   dfcdlAutoScanStartBurst_IN_traffictype traffictype,
    INOUT dfcdlAutoScanStartBurst_FIELDS    *tableFields,
    IN  GT_U32  entryIndex
);
#ifdef __cplusplus 
}
#endif /* __cplusplus */

#endif /* __GtAutoDriverDfcdlh */

