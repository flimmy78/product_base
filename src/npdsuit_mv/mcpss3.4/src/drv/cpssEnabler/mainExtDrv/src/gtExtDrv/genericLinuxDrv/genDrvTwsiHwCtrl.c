/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* genDrvTwsiHwCtrl.c
*
* DESCRIPTION:
*       API implementation for TWSI facilities.
*
* DEPENDENCIES:
*
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/*Includes*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>

#include <gtExtDrv/drivers/gtTwsiHwCtrl.h>
#include <gtExtDrv/drivers/pssBspApis.h>

#include "kerneldrv/include/presteraGlob.h"

/* file descriptor returnd by openning the PP *nix device driver */
extern GT_32 gtPpFd;

/* Macroes definitions */
#define TWSI_SLAVE_ADDR(data)     (data)

#define MV_ON_READ_SET_BITS31(data)    (data[0] |= 0x80)
#define MV_ON_WRITE_RESET_BIT31(data)  (data[0] &= 0x7F)
#define MV_RESET_BIT_30(data)          (data[0] &= 0xBF)

static GT_STATUS mvLongToChar (IN GT_U32 src, 
                               OUT GT_U8 dst[4]);

static GT_STATUS mvCharToLong (IN GT_U8 src[4], 
                               OUT GT_U32 *dst);

static GT_STATUS mvConcatCharArray (IN GT_U8 src0[4], 
                                    IN GT_U8 src1[4], 
                                    OUT GT_U8 dst[8]);

/*******************************************************************************
* hwIfTwsiInitDriver
*
* DESCRIPTION:
*       Init the TWSI interface 
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK     - on success
*       GT_FAIL   - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS hwIfTwsiInitDriver
(
    GT_VOID
)
{
    if (ioctl (gtPpFd, PRESTERA_IOC_TWSIINITDRV) < 0)
    {
        fprintf(stderr, "hwIfTwsiInitDriver fail errno(%s)\n",
                        strerror(errno));
        return GT_FAIL;
    }

    return GT_OK;
}


/*******************************************************************************
* hwIfTwsiWriteReg
*
* DESCRIPTION:
*       Writes the unmasked bits of a register using TWSI.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       regAddr - Register address to write to.
*       value   - Data to be written to register.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK    - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS hwIfTwsiWriteReg
(
    IN GT_U32 devSlvId,
    IN GT_U32 regAddr,
    IN GT_U32 value
)
{
    GT_U8           regCharAddr[4];
    GT_U8           regCharData[4];
    GT_U8           regCharAddrData[8];
    GT_TwsiReadWrite_STC twsiWritePrm;

    /*Phase 1: Master Drives Address and Data over TWSI*/
    mvLongToChar (regAddr, regCharAddr);
    MV_ON_WRITE_RESET_BIT31 (regCharAddr);
    MV_RESET_BIT_30 (regCharAddr);

    mvLongToChar (value, regCharData);
    mvConcatCharArray(regCharAddr,regCharData,regCharAddrData);

    if (ioctl (gtPpFd, PRESTERA_IOC_TWSIWAITNOBUSY) < 0)
    {
        fprintf(stderr, "hwIfTwsiWriteReg(PRESTERA_IOC_TWSIWAITNOBUSY) fail errno(%s)\n",
                        strerror(errno));
        return GT_FAIL;
    }

    twsiWritePrm.devId  = TWSI_SLAVE_ADDR(devSlvId);
    twsiWritePrm.len    = 8;
    twsiWritePrm.pData  = regCharAddrData;
    twsiWritePrm.stop   = GT_TRUE;

    if (ioctl (gtPpFd, PRESTERA_IOC_TWSIWRITE, &twsiWritePrm) < 0)
    {
        fprintf(stderr, "hwIfTwsiWriteReg(PRESTERA_IOC_TWSIWRITE) fail errno(%s)\n",
                        strerror(errno));
        return GT_FAIL;
    }

    return GT_OK;
}


/*******************************************************************************
* hwIfTwsiReadReg
*
* DESCRIPTION:
*       Reads the unmasked bits of a register using TWSI.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       regAddr - Register address to read from.
*
* OUTPUTS:
*       dataPtr    - Data read from register.
*
* RETURNS:
*       GT_OK               - on success
*       GT_ERROR   - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS hwIfTwsiReadReg
(               
    IN  GT_U32  devSlvId,
    IN  GT_U32  regAddr,
    OUT GT_U32 *dataPtr
)
{
    GT_U8           twsiRdDataBuff[4];
    GT_U8           regCharAddr[4];       
    GT_TwsiReadWrite_STC twsiWritePrm;

    /*PHASE 1: Write Structure, Master drives regAddr over TWSI*/
    mvLongToChar(regAddr, regCharAddr);
    MV_ON_READ_SET_BITS31 (regCharAddr);
    MV_RESET_BIT_30 (regCharAddr);
    
    if (ioctl (gtPpFd, PRESTERA_IOC_TWSIWAITNOBUSY) < 0)
    {
        fprintf(stderr, "hwIfTwsiWriteReg(PRESTERA_IOC_TWSIWAITNOBUSY) fail errno(%s)\n",
                        strerror(errno));
        return GT_FAIL;
    }

    twsiWritePrm.devId  = TWSI_SLAVE_ADDR(devSlvId);
    twsiWritePrm.len    = 4;
    twsiWritePrm.pData  = regCharAddr;
    twsiWritePrm.stop   = GT_FALSE;

    if (ioctl (gtPpFd, PRESTERA_IOC_TWSIWRITE, &twsiWritePrm) < 0)
    {
        fprintf(stderr, "hwIfTwsiWriteReg(PRESTERA_IOC_TWSIWRITE) fail errno(%s)\n",
                        strerror(errno));
        return GT_FAIL;
    }

    twsiWritePrm.devId  = TWSI_SLAVE_ADDR(devSlvId);
    twsiWritePrm.len    = 4;
    twsiWritePrm.pData  = twsiRdDataBuff;
    twsiWritePrm.stop   = GT_TRUE;

    if (ioctl (gtPpFd, PRESTERA_IOC_TWSIREAD, &twsiWritePrm) < 0)
    {
        fprintf(stderr, "hwIfTwsiWriteReg(PRESTERA_IOC_TWSIREAD) fail errno(%s)\n",
                        strerror(errno));
        return GT_FAIL;
    }
    
    mvCharToLong(twsiRdDataBuff, dataPtr);

    return GT_OK;
}
 
/*******************************************************************************
* hwIfTwsiWriteData
*
* DESCRIPTION:
*       Generic TWSI Write operation.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       dataPtr  - (pointer to) data to be send on the TWSI.
*       dataLen  - number of bytes to send on the TWSI (from dataPtr).
*                  range: 1-8.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on hardware error
*       GT_BAD_PARAM    - on wrong parameters
*       GT_BAD_PTR      - on null pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS hwIfTwsiWriteData
(
    IN GT_U32 devSlvId,
    IN GT_U8  *dataPtr,
    IN GT_U8  dataLen
)
{
    GT_TwsiReadWrite_STC twsiWritePrm;

    if( dataPtr == NULL )
        return GT_BAD_PTR;

    if( dataLen < 1 || dataLen > 8 )
        return GT_BAD_PARAM;

    if (ioctl (gtPpFd, PRESTERA_IOC_TWSIWAITNOBUSY) < 0)
    {
        fprintf(stderr, "hwIfTwsiWriteReg(PRESTERA_IOC_TWSIWAITNOBUSY) fail errno(%s)\n",
                        strerror(errno));
        return GT_FAIL;
    }

    twsiWritePrm.devId  = TWSI_SLAVE_ADDR(devSlvId);
    twsiWritePrm.len    = dataLen;
    twsiWritePrm.pData  = dataPtr;
    twsiWritePrm.stop   = GT_TRUE;

    if (ioctl (gtPpFd, PRESTERA_IOC_TWSIWRITE, &twsiWritePrm) < 0)
    {
        fprintf(stderr, "hwIfTwsiWriteReg(PRESTERA_IOC_TWSIWRITE) fail errno(%s)\n",
                        strerror(errno));
        return GT_FAIL;
    }

    return GT_OK;
}

/*******************************************************************************
* hwIfTwsiReadData
*
* DESCRIPTION:
*       Generic TWSI Read operation.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       dataLen  - number of bytes to recieve on the TWSI (into dataPtr).
*                  range: 1-8.
*
* OUTPUTS:
*       dataPtr  - (pointer to) data to be send on the TWSI.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on hardware error
*       GT_BAD_PARAM    - on wrong parameters
*       GT_BAD_PTR      - on null pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS hwIfTwsiReadData
(               
    IN GT_U32 devSlvId,
    IN GT_U8  dataLen,
    OUT GT_U8 *dataPtr
)
{
    GT_TwsiReadWrite_STC twsiWritePrm;

    if( dataPtr == NULL )
        return GT_BAD_PTR;

    if( dataLen < 1 || dataLen > 8 )
        return GT_BAD_PARAM;

    if (ioctl (gtPpFd, PRESTERA_IOC_TWSIWAITNOBUSY) < 0)
    {
        fprintf(stderr, "hwIfTwsiWriteReg(PRESTERA_IOC_TWSIWAITNOBUSY) fail errno(%s)\n",
                        strerror(errno));
        return GT_FAIL;
    }

    twsiWritePrm.devId  = TWSI_SLAVE_ADDR(devSlvId);
    twsiWritePrm.len    = dataLen;
    twsiWritePrm.pData  = dataPtr;
    twsiWritePrm.stop   = GT_TRUE;

    if (ioctl (gtPpFd, PRESTERA_IOC_TWSIREAD, &twsiWritePrm) < 0)
    {
        fprintf(stderr, "hwIfTwsiWriteReg(PRESTERA_IOC_TWSIREAD) fail errno(%s)\n",
                        strerror(errno));
        return GT_FAIL;
    }
    
    return GT_OK;
}

/*******************************************************************************
* mvLongToChar
*
* DESCRIPTION:
*       Transforms unsigned long int type to 4 separate chars.
*
* INPUTS:
*       src - source unsigned long integer.
*
* OUTPUTS:
*       dst - Array of 4 chars
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL   - on failure
*
* COMMENTS:
*           MSB is copied to dst[0]!!
*
*******************************************************************************/
static GT_STATUS mvLongToChar (IN GT_U32 src, 
                               OUT GT_U8 dst[4])
{
    GT_U32 i;

    for (i = 4 ; i > 0 ; i--)
    {
        dst[i-1] = (GT_U8) src & 0xFF;
        src>>=8;
    }

  return GT_OK;
}

/*******************************************************************************
* mvCharToLong
*
* DESCRIPTION:
*       Transforms an array of 4 separate chars to unsigned long integer type
*
* INPUTS:
*       src - Source Array of 4 chars
*
* OUTPUTS:
*       dst - Unsigned long integer number
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL   - on failure
*
* COMMENTS:
*           MSB resides in src[0]!!
*
*******************************************************************************/
static GT_STATUS mvCharToLong (IN GT_U8 src[4], 
                               OUT GT_U32 *dst)
{
    GT_U32  i;
    GT_U32  tempU32 = 0x00000000;    

    for (i = 4 ; i > 0 ; i--)
    {
        tempU32 += ((GT_U32)src[i-1]) << (8*(4-i));
    }

  *dst = tempU32;
  return GT_OK;
}


/*******************************************************************************
* mvConcatCharArrays
*
* DESCRIPTION:
*       Concatinate 2 Arrays of Chars to one Array of chars
*
* INPUTS:
*       src0 - Source Array of 4 chars long
*       src1 - Source Array of 4 chars long
*
* OUTPUTS:
*       dst - Conacatinated Array of 8 chars long {src1,src0}
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL   - on failure
*
* COMMENTS:
*           None
*
*******************************************************************************/
static GT_STATUS mvConcatCharArray (IN GT_U8 src0[4], 
                                    IN GT_U8 src1[4], 
                                    OUT GT_U8 dst[8])
{
    GT_U32 i=0,j; /*Source and Dest Counters*/

    for (j=0; j < 8; j++)
    {
        if (j < 4)
            dst[j] = src0[i++];
        else if (j == 4)
        {
            dst[j] = src1[0];
            i=1;
        } else
            dst[j] = src1[i++];
    }
    return GT_OK;
}

