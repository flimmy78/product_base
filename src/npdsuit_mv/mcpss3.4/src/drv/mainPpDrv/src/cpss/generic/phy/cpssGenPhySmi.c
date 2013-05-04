/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
*
* cpssGenPhySmi.c
*
* DESCRIPTION:
*       API implementation for port Core Serial Management Interface facility.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.7 $
*
*******************************************************************************/

#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/phy/private/prvCpssGenPhySmi.h>

/* Add for the define of BOARD_TYPE, zhangdi 2011-04-07 */
#if 0
#include <../../../../../npd/src/app/npd_board.h>
#else
#include <sysdef/npd_sysdef.h>   /* for board_info */
#endif
/* Add end */

#ifndef __AX_PLATFORM__
	extern unsigned int auteware_product_id;
#endif

/*************************** Private definitions ******************************/

/*************************** Private functions ********************************/

/*******************************************************************************
* phy1340Init
*
* DESCRIPTION:
*   This function implements the WA for the 88E1340.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - Device Number.
*       portNum             - Port Number.
*       smiWriteRegister    - pointer SMI Write implementation
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS phy1340Init
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
)
{
	printf("************* phy1340Init() \n");
	printf("************* portNum:%d \n",portNum);
    GT_STATUS     rc;         /* Return code                  */

    /* WA for the short cable problem */
    rc =  smiWriteRegister(devNum, portNum, PRV_CPSS_GE_PHY_EXT_ADDR, 0x00FF);
    if ( rc != GT_OK )
    {
        return rc;
    }

    rc =  smiWriteRegister(devNum, portNum, PRV_CPSS_PHY_REG24, 0x2800);
    if ( rc != GT_OK )
    {
        return rc;
    }

    rc =  smiWriteRegister(devNum, portNum, PRV_CPSS_PHY_REG23, 0x2001);
    if ( rc != GT_OK )
    {
        return rc;
    }

    /* WA for the 88E1340 ESD problem */
    rc =  smiWriteRegister(devNum, portNum, PRV_CPSS_GE_PHY_EXT_ADDR, 0x0000);
    if ( rc != GT_OK )
    {
        return rc;
    }

    rc =  smiWriteRegister(devNum, portNum, PRV_CPSS_PHY_REG29, 0x0010);
    if ( rc != GT_OK )
    {
        return rc;
    }

    rc =  smiWriteRegister(devNum, portNum, PRV_CPSS_PHY_REG30, 0x4018);
    if ( rc != GT_OK )
    {
        return rc;
    }

    rc =  smiWriteRegister(devNum, portNum, PRV_CPSS_PHY_REG29, 0x0000);

    /* Here is the different branch for board */
    if((BOARD_TYPE == BOARD_TYPE_AX81_AC12C)||(BOARD_TYPE == BOARD_TYPE_AX81_AC8C))
    {
        /* LED Function Control Register. Page 3, Register 16 */
        rc =  smiWriteRegister(devNum, portNum, PRV_CPSS_GE_PHY_EXT_ADDR, 0x0003);
        if ( rc != GT_OK )
        {
            return rc;
        }
		
        /* 
         * Here is for AC12C PHY(1340S),because it's Combo interface
         * So, we use led[2-3] for fiber port.
         * LED[3]: 0000 = On - Fiber Link, Off - Else
         * LED[2]: 0100 = Blink - Activity, Off - No Activity
         * LED[1]: On-Link, Blink-Activity, Off-No Link
         * LED[0]: On-1000M Link
         */
        #if 0 
		rc =  smiWriteRegister(devNum, portNum, 16, 0x0417);    /* AX81-AC12C */
        if ( rc != GT_OK )
        {
            return rc;
        }
		#endif
        /* the phy is in copper mode when start up,so we make fiber led off */
		rc =  smiWriteRegister(devNum, portNum, 16, 0x8817);    /* AX81-AC12C */
        if ( rc != GT_OK )
        {
            return rc;
        }

       /* Set the PHY to Combo mode
		* Table 218: General Control Register
        * Page 6, Register 20
        */
		rc =  smiWriteRegister(devNum, portNum, PRV_CPSS_GE_PHY_EXT_ADDR, 0x0006);
        if ( rc != GT_OK )
        {
            return rc;
        }
		#if 0       /* code optmize:  Logically dead code. zhangdi@autelan.com 2013-01-17 */
        /* bit[4,5] = 00 :link on first media */
		rc =  smiWriteRegister(devNum, portNum, 20, 0x8207);
        if ( rc != GT_OK )
        {
            return rc;
        }
		#endif
		
    }
	else if((BOARD_TYPE == BOARD_TYPE_AX71_2X12G12S)||(BOARD_TYPE == BOARD_TYPE_AX81_1X12G12S))
    {
        /* Here is for 2X12G12S */
        /* LED Function Control Register. Page 3, Register 16 */
        rc =  smiWriteRegister(devNum, portNum, PRV_CPSS_GE_PHY_EXT_ADDR, 0x0003);
        if ( rc != GT_OK )
        {
            return rc;
        }
		
        if(portNum<=11)    /* code optmize: "portNum >= 0" is true. zhangdi@autelan.com 2013-01-17 */
        {
           /* 
            * port[0-11] : copper on 1340
			* LED[1]: 0001 = On-Link, Blink-Activity, Off-No Link
            * LED[0]: 0111 = On - 1000 Mbps Link, Off - Else
            */	
			rc =  smiWriteRegister(devNum, portNum, 16, 0x8817);
            if ( rc != GT_OK )
            {
                return rc;
            }	
			
        }
		else
		{
		   /* 
		    * port[12-23]: fiber on 1340S	
			* LED[1]: 0100 = Blink - Activity, Off - No Activity
            * LED[0]: 0111 = On - 1000 Mbps Link, Off - Else
            */
    		rc =  smiWriteRegister(devNum, portNum, 16, 0x8847);
            if ( rc != GT_OK )
            {
                return rc;
            }
		}
    }
	else
	{
		/* do not match any board, force up all led. */
		rc =  smiWriteRegister(devNum, portNum, 16, 0x9999); /* force LED ON */
        if ( rc != GT_OK )
        {
            return rc;
        }
	
	}

	/* Write page back to 0 */
    rc =  smiWriteRegister(devNum, portNum, PRV_CPSS_GE_PHY_EXT_ADDR, 0x0000);
    if ( rc != GT_OK )
    {
        return rc;
    }
    
    return rc;

}

/*******************************************************************************
* phyLockUpprevent
*
* DESCRIPTION:
*   This function implements the WA for the 88E1112 SERDES lock-up erratum
*   (during the initialization section).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - Device Number.
*       portNum             - Port Number.
*       smiReadRegister     - Read function
*       smiWriteRegister    - Write function
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS phyLockUpprevent
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_READ_FUNC  smiReadRegister,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
)
{
    GT_STATUS     rc;         /* Return code                  */
    GT_U16        tempVal;    /* Value to set to the register */

    rc =  smiWriteRegister(devNum,portNum,PRV_CPSS_PHY_REG29,0x0008);

    rc |= smiReadRegister(devNum,portNum,PRV_CPSS_PHY_REG30,&tempVal);
    tempVal = (GT_U16)(tempVal & 0xFFFC);
    rc |= smiWriteRegister(devNum,portNum,PRV_CPSS_PHY_REG30,tempVal);

    rc |= smiWriteRegister(devNum,portNum,PRV_CPSS_PHY_REG29,0x000C);

    rc |= smiReadRegister(devNum,portNum,PRV_CPSS_PHY_REG30,&tempVal);
    tempVal = (GT_U16)(tempVal & 0xFFFC);
    rc |= smiWriteRegister(devNum,portNum,PRV_CPSS_PHY_REG30,tempVal);

    if ( rc != GT_OK )
    {
        return (rc);
    }

    rc = cpssOsTimerWkAfter(1);
    if ( rc != GT_OK )
    {
        return (rc);
    }

    /* Reset SGMII MAC interface SERDES receive block. */
    rc =  smiWriteRegister(devNum,portNum,PRV_CPSS_PHY_REG29,0x0017);
    rc |= smiWriteRegister(devNum,portNum,PRV_CPSS_PHY_REG30,0x4008);
    rc |= smiWriteRegister(devNum,portNum,PRV_CPSS_PHY_REG30,0x0000);

    if ( rc != GT_OK )
    {
        return (rc);
    }

    /* Reset Fiber media interface SERDES receive block. */
    rc =  smiWriteRegister(devNum,portNum,PRV_CPSS_PHY_REG29,0x0018);
    rc |= smiWriteRegister(devNum,portNum,PRV_CPSS_PHY_REG30,0x00CA);
    rc |= smiWriteRegister(devNum,portNum,PRV_CPSS_PHY_REG30,0x0000);

    if ( rc != GT_OK )
    {
        return (rc);
    }

    rc = cpssOsTimerWkAfter(2);
    if ( rc != GT_OK )
    {
        return (rc);
    }

    return (GT_OK);
}

/*******************************************************************************
* phy1149GigabitVodSet
*
* DESCRIPTION:
*   This function implements the WA for the 88E1149 revision C1 gigabit VOD
*   erratum (during the initialization section).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - Device Number.
*       portNum     - Port Number.
*       vodRegValue - value to be set in Vod register
*       smiWriteRegister - pointer SMI Write implementation
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS phy1149GigabitVodSet
(
    IN GT_U8  devNum,
    IN GT_U8  portNum,
    IN GT_U16 vodRegValue,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
)
{
    /* set an access to AFE block*/
    smiWriteRegister (devNum, portNum, PRV_CPSS_GE_PHY_EXT_ADDR, 0x00FF);

    /* For Prestera PP + E1149 rev C1 based systems PHY developers
       advise adjusting to 0x7777 for normal operation (lowest Gig VOD)
      If using the IEEE test mode 1, PHY developers advise writing 0xCCCC
      to pass IEEE gig VOD. */
    smiWriteRegister (devNum, portNum, PRV_CPSS_PHY_REG24, vodRegValue);

    /* indirect write: vodRegValue to register 12 */
    smiWriteRegister (devNum, portNum, PRV_CPSS_PHY_REG23, 0x2012);


    return GT_OK ;
}

/*******************************************************************************
* phy114xErratafix
*
* DESCRIPTION:
*   This function implements the WA for the 88E114X erratum.
*
* INPUTS:
*       devNum    - Device Number.
*       portNum   - Port Number.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
#ifdef __AX_PLATFORM__
static GT_STATUS phy114xErratafix
(
    IN GT_U8 devNum,
    IN GT_U8 portNum
)
{
    return GT_OK;
}
#else
static GT_STATUS phy114xErratafix
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_READ_FUNC  smiReadRegister,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
)
{
    GT_STATUS   status; /* return value */
    GT_U16      tempVal;/* Value to set to the register */

    /* Fiber PHY Reset
      * write register 22 value 1:select fiber banks register 
      */
    /* phase 1 step 1 */
    status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,1);
    if(status != GT_OK)
    {       
        osPrintf("select fiber page write PHY:dev %d port %d reg %d val %#0x error\r\n", \
                    devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,1);
        return status;
    }
    /* read register 0 */
    status = smiReadRegister(devNum, portNum, PRV_CPSS_PHY_CNTRL, &tempVal);
    if (status != GT_OK)
    {
        /* HW problem or PHY not connected to a port */
        osPrintf("read Fiber PHY:dev %d port %d reg %d val %#0x error\r\n", \
                    devNum,portNum,PRV_CPSS_PHY_CNTRL,tempVal);
        return status;
    }
    /* set bit[12]:0 disable Auto-Nego bit[15]:1 PHY reset */
    tempVal &= 0xEFFF; /* bit12:0 */
    tempVal |= 1<<15;  /* bit15:1 */
    status = smiWriteRegister(devNum,portNum, PRV_CPSS_PHY_CNTRL,tempVal);
    if(status != GT_OK)
    {
        osPrintf("write Fiber PHY:dev %d port %d reg %d val %#0x error\r\n", \
                    devNum,portNum,PRV_CPSS_PHY_CNTRL,tempVal);
        return status;
    }
    
    /* Copper PHY Reset 
      *
      * write register 22 value 0:select copper banks register 
      */
    /* phase 2 step 1 */
    tempVal = 0;
    status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,0);
    if(status != GT_OK)
    {       
        osPrintf("select copper page write PHY:dev %d port %d reg %d val %#0x error\r\n", \
                    devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,0);
        return status;
    }
    /* read register 0 */
    status = smiReadRegister(devNum, portNum, PRV_CPSS_PHY_CNTRL, &tempVal);
    if (status != GT_OK)
    {
        /* HW problem or PHY not connected to a port */
        osPrintf("read copper PHY:dev %d port %d reg %d val %#0x error\r\n", \
                    devNum,portNum,PRV_CPSS_PHY_CNTRL,tempVal);
        return status;
    }
    /* set bit[15]:1 PHY reset */
    tempVal |= 1<<15;  /* bit15:1 */
    status = smiWriteRegister(devNum,portNum, PRV_CPSS_PHY_CNTRL,tempVal);
    if(status != GT_OK)
    {
        osPrintf("write copper PHY:dev %d port %d reg %d val %#0x error\r\n", \
                    devNum,portNum,PRV_CPSS_PHY_CNTRL,tempVal);
        return status;
    }
    
    /* write register 27 disable Auto-Selection,
      * write register 0 bit15:1 reset PHY 
      */
    /* phase 3 step 1 */
    tempVal = 0;
    smiReadRegister(devNum, portNum, PRV_CPSS_PHY_EXT_SPEC_STATUS, &tempVal);
    tempVal = 0x9140;
    status = smiWriteRegister(devNum,portNum,PRV_CPSS_PHY_EXT_SPEC_STATUS,tempVal);
    if(status != GT_OK)
    {
        osPrintf("write PHY:dev %d port %d reg %d val %#0x error\r\n", \
                    devNum,portNum,PRV_CPSS_PHY_EXT_SPEC_STATUS,tempVal);
        return status;
    }
    /* phase 3 step 2 */
    /* before write register 0, read first */
    tempVal = 0;
    status = smiReadRegister(devNum,portNum, PRV_CPSS_PHY_CNTRL, &tempVal);
    if(status != GT_OK)
    {
        osPrintf("read PHY dev %d port %d reg %d val %#0x error\r\n", \
                devNum,portNum,PRV_CPSS_PHY_CNTRL,tempVal);
        return status;
    }

    /* write register 0 bit15 to 1 */
    tempVal |= 1<<15; /* bit15:1 */
    status = smiWriteRegister(devNum,portNum, PRV_CPSS_PHY_CNTRL, tempVal);
    if(status != GT_OK)
    {
        osPrintf("write PHY dev %d port %d reg %d val %#0x error\r\n", \
                devNum,portNum,PRV_CPSS_PHY_CNTRL,tempVal);
        return status;
    }

    /* write register 20 bit13:1,PHY power up
      * write register 26 bit[6:5]:0,set Fiber input/output impedance 50 ohm
      * write register 22 value 0:select copper banks register 
      * write register 0 bit15:1 reset PHY 
      */
    /* phase 4 step 1 */
    tempVal = 0;
    status = smiReadRegister(devNum,portNum, PRV_CPSS_PHY_EXT_SPEC_CNTRL, &tempVal);
    if(status != GT_OK)
    {
        osPrintf("read PHY dev %d port %d reg %d val %#0x error\r\n", \
                devNum,portNum,PRV_CPSS_PHY_EXT_SPEC_CNTRL,tempVal);
        return status;
    }

    /* write register 20 bit13 to 0 */
    tempVal &= 0xDFFF; /* bit13:0 */
    status = smiWriteRegister(devNum,portNum, PRV_CPSS_PHY_EXT_SPEC_CNTRL, tempVal);
    if(status != GT_OK)
    {
        osPrintf("write PHY dev %d port %d reg %d val %#0x error\r\n", \
                devNum,portNum,PRV_CPSS_PHY_EXT_SPEC_CNTRL,tempVal);
        return status;
    }

    /* phase 4 step 2 */
    tempVal = 0;
    status = smiReadRegister(devNum,portNum, PRV_CPSS_PHY_EXT_SPEC_CNTRL2, &tempVal);
    if(status != GT_OK)
    {
        osPrintf("read PHY dev %d port %d reg %d val %#0x error\r\n", \
                devNum,portNum,PRV_CPSS_PHY_EXT_SPEC_CNTRL2,tempVal);
        return status;
    }

    /* write register 26 bit[6:5] to 0 */
    tempVal &= 0xFF9F; /* bit[6:5]:0 */
    status = smiWriteRegister(devNum,portNum, PRV_CPSS_PHY_EXT_SPEC_CNTRL2, tempVal);
    if(status != GT_OK)
    {
        osPrintf("write PHY dev %d port %d reg %d val %#0x error\r\n", \
                devNum,portNum,PRV_CPSS_PHY_EXT_SPEC_CNTRL2,tempVal);
        return status;
    }

    /* phase 4 step 3 */
    /* write register 22 value 0: select copper banks register */
    tempVal = 0;
    status = smiWriteRegister(devNum,portNum, PRV_CPSS_GE_PHY_EXT_ADDR, tempVal);
    if(status != GT_OK)
    {
        osPrintf("write PHY dev %d port %d reg %d val %#0x error\r\n", \
                devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
        return status;
    }

    /* phase 4 step 4 */
    tempVal = 0;
    status = smiReadRegister(devNum,portNum, PRV_CPSS_PHY_CNTRL, &tempVal);
    if(status != GT_OK)
    {
        osPrintf("read PHY dev %d port %d reg %d val %#0x error\r\n", \
                devNum,portNum,PRV_CPSS_PHY_CNTRL,tempVal);
        return status;
    }

    /* write register 0 bit15:1 */
    tempVal |= 1<<15; /* bit1:1 */
    status = smiWriteRegister(devNum,portNum, PRV_CPSS_PHY_CNTRL, tempVal);
    if(status != GT_OK)
    {
        osPrintf("write PHY dev %d port %d reg %d val %#0x error\r\n", \
                devNum,portNum,PRV_CPSS_PHY_CNTRL,tempVal);
        return status;
    }

    /* write register 24 bit0:1,set LED_TX_control(MSB) is a 0
      */
    /* phase 5 step 1 */
    tempVal = 0;
    status = smiReadRegister(devNum,portNum, PRV_CPSS_PHY_LED_CNTRL, &tempVal);
    if(status != GT_OK)
    {
        osPrintf("read PHY dev %d port %d reg %d val %#0x error\r\n", \
                devNum,portNum,PRV_CPSS_PHY_LED_CNTRL,tempVal);
        return status;
    }

    /* write register 24 bit0:1 */
    tempVal |= 1; /* bit0:1 */
    status = smiWriteRegister(devNum,portNum, PRV_CPSS_PHY_LED_CNTRL, tempVal);
    if(status != GT_OK)
    {
        osPrintf("write PHY dev %d port %d reg %d val %#0x error\r\n", \
                devNum,portNum,PRV_CPSS_PHY_LED_CNTRL,tempVal);
        return status;
    }

    return status;
}
#endif

#ifndef __AX_PLATFORM__
static GT_STATUS phy1112ConfigFix
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_READ_FUNC  smiReadRegister,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
)
{
	GT_STATUS 	status; /* return value */
	GT_U16 		tempVal;/* Value to set to the register */

	/*
	  * At first, Power down copper and fiber,and then reset 
	  */
	/* phase 0 step 1 *//* copper */
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,0);
	if(status != GT_OK) {		
		osPrintf("phy 1112 power down copper select page write PHY:dev %d port %d reg %d val %#0x error %d\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,0,status);
		return status;
	}
	/* read register 0 */
    	status = smiReadRegister(devNum, portNum, PRV_CPSS_PHY_CNTRL, &tempVal);
    	if (status != GT_OK) {
        	/* HW problem or PHY not connected to a port */
		osPrintf("phy 1112 power down copper read PHY:dev %d port %d reg %d val %#0x error %d\n", \
					devNum,portNum,PRV_CPSS_PHY_CNTRL,tempVal,status);
        	return status;
    	}
	/* set bit[11]:1 Power Down 
	 *  set bit[15]:1 PHY reset 
	 */
	tempVal |= 0x8800;
	status = smiWriteRegister(devNum,portNum, PRV_CPSS_PHY_CNTRL,tempVal);
	if(status != GT_OK) {
		osPrintf("phy 1112 power down copper write PHY:dev %d port %d reg %d val %#0x error %d\n", \
					devNum,portNum,PRV_CPSS_PHY_CNTRL,tempVal,status);
		return status;
	}
	/* Default MAC interface and SGMII media interface speed
	  * write register 22 value 2:select page 2 register 0 
	  */
	/* phase 1 step 1 */
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,2);
	if(status != GT_OK) {		
		osPrintf("phy 1112 reset MAC control select page write PHY:dev %d port %d reg %d val %#0x error %d\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,2,status);
		return status;
	}
	/* read register 0 */
    	status = smiReadRegister(devNum, portNum, PRV_CPSS_PHY_CNTRL, &tempVal);
    	if (status != GT_OK) {
        	/* HW problem or PHY not connected to a port */
		osPrintf("phy 1112 reset MAC control read PHY:dev %d port %d reg %d val %#0x error %d\n", \
					devNum,portNum,PRV_CPSS_PHY_CNTRL,tempVal,status);
        	return status;
    	}
	/* set bit[12]:0 disable Auto-Nego bit[15]:1 PHY reset 
	 * set bit6,13 -> 10:non - 10Mbps speed
	 */
	tempVal = 0x8040;
	status = smiWriteRegister(devNum,portNum, PRV_CPSS_PHY_CNTRL,tempVal);
	if(status != GT_OK) {
		osPrintf("phy 1112 reset MAC control write PHY:dev %d port %d reg %d val %#0x error %d\n", \
					devNum,portNum,PRV_CPSS_PHY_CNTRL,tempVal,status);
		return status;
	}
	/* LOS INIT STATUS[1:0] function control
	  *  bit[15,12]:0111: LOS control
	  *  bit[11:8]:0100: INIT control
	  *  bit[7:4]:0001: STATUS[1] control
	  *  bit[3:0]:0111: STATUS[0] control
	  *
	  * write register 22 value 3:select page 3
	  */
	/* phase 2 step 1 */
	tempVal = 0;
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,3);
	if(status != GT_OK) {		
		osPrintf("phy 1112 LOS STATUS select page write PHY:dev %d port %d reg %d val %#0x error %d\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,3,status);
		return status;
	}
	/* set
	  *  bit[15,12]:0111: LOS control
	  *  bit[11:8]:0100: INIT control
	  *  bit[7:4]:0001: STATUS[1] control
	  *  bit[3:0]:0111: STATUS[0] control
	  *
	  *	when combo Auto-Media work under copper mode, set value 0x7817
	  *	when combo Auto-Media work under fiber mode, set value 0x7488
	  */
	tempVal = 0x7817;
	status = smiWriteRegister(devNum,portNum, 16,tempVal);
	if(status != GT_OK) {
		osPrintf("phy 1112 LOS STATUS write PHY:dev %d port %d reg %d val %#0x error %d\n", \
					devNum,portNum,16,tempVal,status);
		return status;
	}

	/* LOS INIT STATUS[1:0] timer control
	  *  bit[14,12]:100: pulse stretch duration - 170ms to 340ms
	  *  bit[10:8]:001: blink rate - 84ms
	  *  bit[3:2]:0001: speed off pulse period - 170ms
	  *  bit[1:0]:0111: speed on pulse period - 170ms
	  *
	  * write register 22 value 3:select page 3
	  */
	/* phase 3 step 1 */
	tempVal = 0x4105;
	status = smiWriteRegister(devNum,portNum, 18,tempVal);
	if(status != GT_OK) {
		osPrintf("phy 1112 LOS STATUS write PHY:dev %d port %d reg %d val %#0x error %d\n", \
					devNum,portNum,18,tempVal,status);
		return status;
	}
	/* MAC Specific Control Register 1 
	  * bit[9:7]:011: copper/1000BASE-X Auto Selection
	  * write page 2 register 16 bit[9:7]:011:copper/1000BASE-X Auto 
	  */
	/* phase 4 step 1 */
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,2);
	if(status != GT_OK) {		
		osPrintf("phy 1112 media auto select page write PHY:dev %d port %d reg %d val %#0x error %d\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,2,status);
		return status;
	}
	/* read register 16 */
    	status = smiReadRegister(devNum, portNum, 16, &tempVal);
    	if (status != GT_OK) {
        	/* HW problem or PHY not connected to a port */
		osPrintf("phy 1112 media auto read PHY:dev %d port %d reg %d val %#0x error %d\n", \
					devNum,portNum,16,tempVal,status);
        	return status;
    	}
	/* set bit[9:7]:011: copper/1000BASE-X Auto 
	 */
	tempVal &= 0xFC7F; /* clear bit[9:7] */
	tempVal |= (0x3 << 7); /* set bit[9:7] = 0x3 */
	status = smiWriteRegister(devNum,portNum, 16,tempVal);
	if(status != GT_OK) {
		osPrintf("phy 1112 media auto write PHY:dev %d port %d reg %d val %#0x error %d\n", \
					devNum,portNum,16,tempVal,status);
		return status;
	}

	/* Fiber Specific Control Register 1 
	  * bit[9]:1:  SIGDET low means signal,high means no signal
	  * write page 1 register 16 bit[9]:1:SIGDET mode 
	  */
	/* phase 5 step 1 */
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,1);
	if(status != GT_OK) {		
		osPrintf("phy 1112 SIGDET select page write PHY:dev %d port %d reg %d val %#0x error %d\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,1,status);
		return status;
	}
	/* read register 16 */
    	status = smiReadRegister(devNum, portNum, 16, &tempVal);
    	if (status != GT_OK) {
        	/* HW problem or PHY not connected to a port */
		osPrintf("phy 1112 SIGDET read PHY:dev %d port %d reg %d val %#0x error %d\n", \
					devNum,portNum,16,tempVal,status);
        	return status;
    	}
	/* set bit[9]:1: SIGDET mode 
	 */
	tempVal &= 0xFDFF; /* clear bit[9] */
	tempVal |= (0x1 << 9); /* set bit[9] = 1 */
	status = smiWriteRegister(devNum,portNum, 16,tempVal);
	if(status != GT_OK) {
		osPrintf("phy 1112 SIGDET write PHY:dev %d port %d reg %d val %#0x error %d\n", \
					devNum,portNum,16,tempVal,status);
		return status;
	}

	/* Fiber Control Register
	  * Set 1000BASE-X Auto-Nego enable and Reset PHY
	  * write page 1 register 0 value 0x9140: 1000BASE-X Auto-Nego enable and reset PHY 
	  */
	/* phase 6 step 1 */
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,1);
	if(status != GT_OK) {		
		osPrintf("phy 1112 fiber AN select page write PHY:dev %d port %d reg %d val %#0x error %d\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,1,status);
		return status;
	}

	tempVal = 0x9140; 
	status = smiWriteRegister(devNum,portNum, PRV_CPSS_PHY_CNTRL,tempVal);
	if(status != GT_OK) {
		osPrintf("phy 1112 fiber AN write PHY:dev %d port %d reg %d val %#0x error %d\n", \
					devNum,portNum,PRV_CPSS_PHY_CNTRL,tempVal,status);
		return status;
	}
	/*
	  * At last, Power on copper and fiber,and then reset 
	  */
	/* phase 7 step 1 *//* copper */
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,0);
	if(status != GT_OK) {		
		osPrintf("phy 1112 power on copper select page write PHY:dev %d port %d reg %d val %#0x error %d\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,0,status);
		return status;
	}
	/* read register 0 */
    	status = smiReadRegister(devNum, portNum, PRV_CPSS_PHY_CNTRL, &tempVal);
    	if (status != GT_OK) {
        	/* HW problem or PHY not connected to a port */
		osPrintf("phy 1112 power on copper read PHY:dev %d port %d reg %d val %#0x error %d\n", \
					devNum,portNum,PRV_CPSS_PHY_CNTRL,tempVal,status);
        	return status;
    	}
	/* set bit[11]:0 Normal Operation 
	 *  set bit[15]:1 PHY reset 
	 */
	tempVal &= 0xF7FF;
	tempVal |= 0x8000;
	status = smiWriteRegister(devNum,portNum, PRV_CPSS_PHY_CNTRL,tempVal);
	if(status != GT_OK) {
		osPrintf("phy 1112 power on copper write PHY:dev %d port %d reg %d val %#0x error %d\n", \
					devNum,portNum,PRV_CPSS_PHY_CNTRL,tempVal,status);
		return status;
	}
	
	/*
	  *  Read phy status 
	  *
	  */
	/* phase 8 step 1 */ /* copper */
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,0);
	if(status != GT_OK) {		
		osPrintf("phy 1112 read status select page write PHY:dev %d port %d reg %d val %#0x error %d\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,0,status);
		return status;
	}
	/* read register 1 */
	status = smiReadRegister(devNum, portNum, PRV_CPSS_PHY_STATUS, &tempVal);
	if (status != GT_OK) {
		/* HW problem or PHY not connected to a port */
		osPrintf("phy 1112 read PHY status:dev %d port %d reg %d val %#0x error %d\n", \
					devNum,portNum,PRV_CPSS_PHY_STATUS,tempVal,status);
		return status;
	}
	/* phase 8 step 2 *//* fiber */
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,1);
	if(status != GT_OK) {		
		osPrintf("phy 1112 read status select page write PHY:dev %d port %d reg %d val %#0x error %d\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,1,status);
		return status;
	}
	/* read register 1 */
	status = smiReadRegister(devNum, portNum, PRV_CPSS_PHY_STATUS, &tempVal);
	if (status != GT_OK) {
		/* HW problem or PHY not connected to a port */
		osPrintf("phy 1112 read PHY status:dev %d port %d reg %d val %#0x error %d\n", \
					devNum,portNum,PRV_CPSS_PHY_STATUS,tempVal,status);
		return status;
	}
	/* phase 8 step 3 *//* enable auto-media register selection */
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,0x8000); 
	if(status != GT_OK) {
		osPrintf("phy 1112 enable auto-media register selection write dev %d port %d reg %d val %#0x error %d\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR, 0x8000,status);
		return status;
	}
    return status;
}

static GT_STATUS phy1240ConfigFix
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_READ_FUNC  smiReadRegister,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
)
{
    GT_STATUS 	status; /* return value */
    GT_U16 		tempVal;/* Value to set to the register */

	/* LOS INIT STATUS[1:0] function control
	  *  bit[15,12]:0111: LOS control
	  *  bit[11:8]:0100: INIT control
	  *  bit[7:4]:0001: STATUS[1] control
	  *  bit[3:0]:0111: STATUS[0] control
	  *
	  * write register 22 value 3:select page 3
	  */
	/* phase 1 step 1 */
	tempVal = 3;
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
	if(status != GT_OK)
	{		
		osPrintf("phase 1 step 1 phy 1240 select page write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,3);
		return status;
	}
	/* set
	  *  bit[15,12]:0111: LOS control
	  *  bit[11:8]:0100: INIT control
	  *  bit[7:4]:0001: STATUS[1] control
	  *  bit[3:0]:0111: STATUS[0] control
	  */
	tempVal = 0x7417;
	status = smiWriteRegister(devNum,portNum, 16,tempVal);
	if(status != GT_OK)
	{
		osPrintf("phase 1 step 2 phy 1240 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,16,tempVal);
		return status;
	}

	/* LOS INIT STATUS[1:0] timer control
	  *  bit[14,12]:100: pulse stretch duration - 170ms to 340ms
	  *  bit[10:8]:001: blink rate - 84ms
	  *  bit[3:2]:0001: speed off pulse period - 170ms
	  *  bit[1:0]:0111: speed on pulse period - 170ms
	  *
	  * write register 22 value 3:select page 3
	  */
	/* phase 2 step 1 */
	tempVal = 0x4105;
	status = smiWriteRegister(devNum,portNum, 18,tempVal);
	if(status != GT_OK)
	{
		osPrintf("phase 2 step 1 phy 1240 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,18,tempVal);
		return status;
	}

	/* 
	 *  Disable MAC Auto-Negotiation
	 *  write page 2 register 21 bit12:0
	 */
	/* phase 3 step 1 : select page 2 */
	 tempVal = 2;
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
	if(status != GT_OK) 
	{
		osPrintf("phase 3 step 1 phy 1240 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
		return status;
	}
	/* phase 3 step 2 : write Auto-Nego bit disable */
	tempVal = 0;
	status = smiReadRegister(devNum,portNum,21,&tempVal);
	if(status != GT_OK)
	{
		osPrintf("phase 3 step 2 phy 1240 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,21,tempVal);
		return status;
	}
	tempVal &= 0xEFFF; /* set bit12 to 0 */
	status = smiWriteRegister(devNum,portNum, 21,tempVal);
	if(status != GT_OK)
	{
		osPrintf("phase 3 step 2 phy 1240 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,21,tempVal);
		return status;
	}
	/* phase 3 step 3 : reset phy */
	tempVal = 0;
	status = smiReadRegister(devNum,portNum,PRV_CPSS_PHY_CNTRL,&tempVal);
	if(status != GT_OK) 
	{
		osPrintf("phase 3 step 3 phy 1240 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,PRV_CPSS_PHY_CNTRL,tempVal);
	}	
	tempVal |= 1<<15;  /* bit15:1 */
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_PHY_CNTRL,tempVal);
	if(status != GT_OK) 
	{
		osPrintf("phase 3 step 3 phy 1240 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,PRV_CPSS_PHY_CNTRL,tempVal);
	}	

	/*
	  *  Read phy status 
	  *
	  */
	/* phase 4 step 1 */
	/* read register 1 */
	status = smiReadRegister(devNum, portNum, PRV_CPSS_PHY_STATUS, &tempVal);
	if (status != GT_OK)
	{
		/* HW problem or PHY not connected to a port */
		osPrintf("phase 4 step 1 phy 1240 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,PRV_CPSS_PHY_STATUS,tempVal);
		return status;
	}

	/*
	 *  Set phy copper wire transmit type
	 */
	/* phase 5 step 1 */
	/* page 0 register 26, 1000BASE-T transmit type -ClassA */
	tempVal = 0;
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
	if(status != GT_OK)
	{		
		osPrintf("phase 5 step 1 phy 1240 select page write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,0);
		return status;
	}
	status = smiReadRegister(devNum, portNum, 26, &tempVal);
	if (status != GT_OK)
	{
		/* HW problem or PHY not connected to a port */
		osPrintf("phase 5 step 2 phy 1240 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,26,tempVal);
		return status;
	}
	tempVal |= (1<<15); /* bit15 -> 1 */
	status = smiWriteRegister(devNum,portNum,26,tempVal);
	if(status != GT_OK)
	{		
		osPrintf("phase 5 step 3 phy 1240 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,26,tempVal);
		return status;
	}
	
    return status;
}

/*******************************************************************************
* phy1112LinkForceUp
*
* DESCRIPTION:
*   This function implements the force link up operation on phy port type 88E1112
*
* INPUTS:
*       devNum    - Device Number.
*       portNum   - Port Number.
*		enable 	  - Set or Clear operation.
*		smiReadRegister - Phy port SMI read function.
*		smiWriteRegister - Phy port SMI write function.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS phy1112LinkForceUp
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    IN GT_U8 enable,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_READ_FUNC  smiReadRegister,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
)
{
	GT_STATUS	status = 0; /* return value */
	GT_U16		tempVal = 0, old_page = 0; /* Value to set to the register */

	/*
	 * Disable PHY Auto-Negotiation
	 */
	PRV_CPSS_PP_MAC(devNum)->phyInfo.genVctBindFunc.cpssPhyAutoNegotiationSet( \
					devNum, portNum, GT_FALSE);
	/* backup old page value */
	status = smiReadRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,&old_page);
	if(GT_OK != status) {
		osPrintfErr("phy 1112 backup page read PHY:dev %d port %d reg %d val %#0x error %d\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,old_page,status);
		goto restore_page;
	}

	/* Copper Specific Control register 0
	  *  bit[10]:1: Force Link Good
	  *
	  * write register 22 value 0:select page 0
	  */
	/* phase 1 step 1 */
	tempVal = 0;
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
	if(status != GT_OK)
	{		
		osPrintf("PHY1112 link force up phase 1-1 select page write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
		goto restore_page;
	}
	/* set
	  *  bit[10]:1: Force Link Good
	  */
	status = smiReadRegister(devNum, portNum, 16, &tempVal);
	if (status != GT_OK)
	{
		/* HW problem or PHY not connected to a port */
		osPrintf("PHY1112 link force up phase 1-2 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,16,tempVal);
		goto restore_page;
	}
	if(enable) { /* set link force UP */
		tempVal |= (1<<10);
	}
	else { /* clear link force UP */
		tempVal &= 0xFBFF;
	}
	status = smiWriteRegister(devNum,portNum, 16,tempVal);
	if(status != GT_OK)
	{
		osPrintf("PHY1112 link force up phase 1-3 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,16,tempVal);
		goto restore_page;
	}
	/* set
	  *  Reg 0 page 0
	  *  bit[11]:0: Normal operation
	  */  
	status = smiReadRegister(devNum, portNum, 0, &tempVal);
	if (status != GT_OK)
	{
		/* HW problem or PHY not connected to a port */
		osPrintf("PHY1112 link force up phase 1-4 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,0,tempVal);
		goto restore_page;
	}
	tempVal &= 0xF7FF;
	status = smiWriteRegister(devNum,portNum, 0,tempVal);
	if(status != GT_OK)
	{
		osPrintf("PHY1112 link force up phase 1-5 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,0,tempVal);
		goto restore_page;
	}
restore_page:	
	/* restore old page selection */
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,old_page);
	if(GT_OK != status) {
		osPrintfErr("phy 1112 restore page write PHY:dev %d port %d reg %d data %#0x error %d\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,old_page,status);
		return status;
	}
	/*
	 * Enable PHY Auto-Negotiation
	 */
	PRV_CPSS_PP_MAC(devNum)->phyInfo.genVctBindFunc.cpssPhyAutoNegotiationSet( \
					devNum, portNum, GT_TRUE);
	return status;
}

/*******************************************************************************
* phy1112LinkForceDown
*
* DESCRIPTION:
*   This function implements the force link down operation on phy port type 88E1112
*
* INPUTS:
*       devNum    - Device Number.
*       portNum   - Port Number.
*		enable 	  - Set or Clear operation.
*		smiReadRegister - Phy port SMI read function.
*		smiWriteRegister - Phy port SMI write function.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS phy1112LinkForceDown
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    IN GT_U8 enable,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_READ_FUNC  smiReadRegister,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
)
{
	GT_STATUS   status = 0; /* return value */
	GT_U16		tempVal = 0, old_page = 0;/* Value to set to the register */

	/*
	 * Disable PHY Auto-Negotiation
	 */
	PRV_CPSS_PP_MAC(devNum)->phyInfo.genVctBindFunc.cpssPhyAutoNegotiationSet( \
					devNum, portNum, GT_FALSE);
	/* backup old page value */
	status = smiReadRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,&old_page);
	if(GT_OK != status) {
		osPrintfErr("phy 1112 backup page read PHY:dev %d port %d reg %d val %#0x error %d\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,old_page,status);
		goto restore_page;
	}

	/* Copper Specific Control register 0
	  *  bit[11]:1: Power Down
	  *
	  * write register 22 value 0:select page 0
	  */
	/* phase 1 step 1 */
	tempVal = 0;
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
	if(status != GT_OK)
	{		
		osPrintf("PHY1112 link force up phase 1-1 select page write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
		goto restore_page;
	}
	/* set
	  *  Reg 0
	  *  bit[11]:1: Power down
	  */
	status = smiReadRegister(devNum, portNum, 0, &tempVal);
	if (status != GT_OK)
	{
		/* HW problem or PHY not connected to a port */
		osPrintf("PHY1112 link force up phase 1-2 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,0,tempVal);
		goto restore_page;
	}
	if(enable) { /* set link force down */
		tempVal |= (1<<11);
	}
	else { /* clear link force down - bit 11*/
			/*set restart Auto-Negotiation - bit 9*/
			/*set software reset - bit 15*/
		status = smiReadRegister(devNum, portNum, 0, &tempVal);
		if (status != GT_OK){
			/* HW problem or PHY not connected to a port */
			osPrintf("PHY1112 link force up phase 1-3 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,0,tempVal);
			goto restore_page;
	    }
		tempVal &= 0x75FF;
	}
	status = smiWriteRegister(devNum,portNum, 0,tempVal);
	if(status != GT_OK)
	{
		osPrintf("PHY1112 link force up phase 1-4 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,0,tempVal);
		goto restore_page;
	}
restore_page:	
	/* restore old page selection */
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,old_page);
	if(GT_OK != status) {
		osPrintfErr("phy 1112 restore page write PHY:dev %d port %d reg %d data %#0x error %d\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,old_page,status);
		return status;
	}
	/*
	 * Enable PHY Auto-Negotiation
	 */
	PRV_CPSS_PP_MAC(devNum)->phyInfo.genVctBindFunc.cpssPhyAutoNegotiationSet( \
					devNum, portNum, GT_TRUE);
	return status;

}

/*******************************************************************************
* phy114xLinkForceUp
*
* DESCRIPTION:
*   This function implements the force link up operation on phy port type 88E114x
*
* INPUTS:
*       devNum    - Device Number.
*       portNum   - Port Number.
*		enable 	  - Set or Clear operation.
*		smiReadRegister - Phy port SMI read function.
*		smiWriteRegister - Phy port SMI write function.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS phy114xLinkForceUp
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    IN GT_U8 enable,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_READ_FUNC  smiReadRegister,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
)
{
	GT_STATUS	status; /* return value */
	GT_U16		tempVal;/* Value to set to the register */

	/* Copper Specific Control register 1
	  *  bit[10]:1: Force Link Good
	  *
	  * write register 22 value 0:select page 0
	  */
	/* phase 1 step 1 */
	tempVal = 0;
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
	if(status != GT_OK)
	{		
		osPrintf("PHY124x link force up phase 1-1 select page write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
		return status;
	}
	/* set
	  *  bit[10]:1: Force Link Good
	  */
	status = smiReadRegister(devNum, portNum, 16, &tempVal);
	if (status != GT_OK)
	{
		/* HW problem or PHY not connected to a port */
		osPrintf("PHY124x link force up phase 1-2 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,16,tempVal);
		return status;
	}
	if(enable) { /* set link force UP */
		tempVal |= (1<<10);
	}
	else { /* clear link force UP */
		tempVal &= 0xFBFF;
	}
	status = smiWriteRegister(devNum,portNum, 16,tempVal);
	if(status != GT_OK)
	{
		osPrintf("PHY124x link force up phase 1-3 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,16,tempVal);
		return status;
	}

	return status;

}

/*******************************************************************************
* phy114xLinkForceDown
*
* DESCRIPTION:
*   This function implements the force link down operation on phy port type 88E114x
*
* INPUTS:
*       devNum    - Device Number.
*       portNum   - Port Number.
*		enable 	  - Set or Clear operation.
*		smiReadRegister - Phy port SMI read function.
*		smiWriteRegister - Phy port SMI write function.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS phy114xLinkForceDown
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    IN GT_U8 enable,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_READ_FUNC  smiReadRegister,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
)
{
	GT_STATUS	status; /* return value */
	GT_U16		tempVal;/* Value to set to the register */

	/* Copper Specific Control register 0
	  *  bit[11]:1: Power Down
	  *
	  * write register 22 value 0:select page 0
	  */
	/* phase 1 step 1 */
	tempVal = 0;
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
	if(status != GT_OK)
	{		
		osPrintf("PHY114x link force down phase 1-1 select page write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
		return status;
	}
	/* set
	  *  Reg 0
	  *  bit[11]:1: Power down
	  */
	status = smiReadRegister(devNum, portNum, 0, &tempVal);
	if (status != GT_OK)
	{
		/* HW problem or PHY not connected to a port */
		osPrintf("PHY114x link force down phase 1-2 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,0,tempVal);
		return status;
	}
	if(enable) { /* set link force down */
		tempVal |= (1<<11);
	}
	else { /* clear link force down */
		tempVal &= 0xF7FF;
	}
	status = smiWriteRegister(devNum,portNum, 0,tempVal);
	if(status != GT_OK)
	{
		osPrintf("PHY114x link force down phase 1-3 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,0,tempVal);
		return status;
	}
	return status;
}


/*******************************************************************************
* phy1240LinkForceUp
*
* DESCRIPTION:
*   This function implements the force link up operation on phy port type 88E1240
*
* INPUTS:
*       devNum    - Device Number.
*       portNum   - Port Number.
*		enable 	  - Set or Clear operation.
*		smiReadRegister - Phy port SMI read function.
*		smiWriteRegister - Phy port SMI write function.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS phy1240LinkForceUp
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    IN GT_U8 enable,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_READ_FUNC  smiReadRegister,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
)
{
    GT_STATUS 	status; /* return value */
    GT_U16 		tempVal;/* Value to set to the register */

	/* Copper Specific Control register 1
	  *  bit[10]:1: Force Link Good
	  *
	  * write register 22 value 0:select page 0
	  */
	/* phase 1 step 1 */
	tempVal = 0;
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
	if(status != GT_OK)
	{		
		osPrintf("PHY1240 link force up phase 1-1 select page write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
		return status;
	}
	/* set
	  *  bit[10]:1: Force Link Good
	  */
	status = smiReadRegister(devNum, portNum, 16, &tempVal);
	if (status != GT_OK)
	{
		/* HW problem or PHY not connected to a port */
		osPrintf("PHY1240 link force up phase 1-2 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,16,tempVal);
		return status;
	}
	if(enable) { /* set link force UP */
		tempVal |= (1<<10);
	}
	else { /* clear link force UP */
		tempVal &= 0xFBFF;
	}
	status = smiWriteRegister(devNum,portNum, 16,tempVal);
	if(status != GT_OK)
	{
		osPrintf("PHY1240 link force up phase 1-3 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,16,tempVal);
		return status;
	}
	/* set
	  *  Reg 0
	  *  bit[11]:0: Normal operation
	  */
	status = smiReadRegister(devNum, portNum, 0, &tempVal);
	if (status != GT_OK)
	{
		/* HW problem or PHY not connected to a port */
		osPrintf("PHY1240 link force up phase 1-2 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,0,tempVal);
		return status;
	}
	tempVal &= 0xF7FF;
	status = smiWriteRegister(devNum,portNum, 0,tempVal);
	if(status != GT_OK)
	{
		osPrintf("PHY1240 link force up phase 1-3 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,0,tempVal);
		return status;
	}
	/* set
	  *  Reg 16
	  *  bit[2]:0: Normal operation
	  */
	status = smiReadRegister(devNum, portNum, 16, &tempVal);
	if (status != GT_OK)
	{
		/* HW problem or PHY not connected to a port */
		osPrintf("PHY1240 link force up phase 1-2 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,16,tempVal);
		return status;
	}
	tempVal &= 0xFFFB;
	status = smiWriteRegister(devNum,portNum, 16,tempVal);
	if(status != GT_OK)
	{
		osPrintf("PHY1240 link force up phase 1-3 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,16,tempVal);
		return status;
	}
	return status;
}
/*******************************************************************************
* phy1340LinkForceUp
*
* DESCRIPTION:
*   This function implements the force link up operation on phy port type 88E1240
*
* INPUTS:
*       devNum    - Device Number.
*       portNum   - Port Number.
*		enable 	  - Set or Clear operation.
*		smiReadRegister - Phy port SMI read function.
*		smiWriteRegister - Phy port SMI write function.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS phy1340LinkForceUp
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    IN GT_U8 enable,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_READ_FUNC  smiReadRegister,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
)
{
    GT_STATUS 	status; /* return value */
    GT_U16 		tempVal;/* Value to set to the register */

	/* Copper Specific Control register 1
	  *  bit[10]:1: Force Link Good
	  *
	  * write register 22 value 0:select page 0
	  */
	/* phase 1 step 1 */
    #if 1
	/* do not write page to 0,for copper & fiber is different */
	status = smiReadRegister(devNum, portNum, PRV_CPSS_GE_PHY_EXT_ADDR, &tempVal);
	if (status != GT_OK)
	{
		/* Read the current phy page */
		osPrintfErr("PHY1340 Read page :dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
		return status;
	}
	osPrintfDbg("PHY1340 Read page :dev %d port %d reg %d val %#0x OK\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
    #else
	tempVal = 0;
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
	if(status != GT_OK)
	{		
		osPrintf("PHY1340 link force up phase 1-1 select page write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
		return status;
	}
    #endif	
	/* set
	  *  bit[10]:1: Force Link Good
	  */
	status = smiReadRegister(devNum, portNum, 16, &tempVal);
	if (status != GT_OK)
	{
		/* HW problem or PHY not connected to a port */
		osPrintfErr("PHY1340 link force up phase 1-2 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,16,tempVal);
		return status;
	}
	if(enable) { /* set link force UP */
		tempVal |= (1<<10);
	}
	else { /* clear link force UP */
		tempVal &= 0xFBFF;
	}
	status = smiWriteRegister(devNum,portNum, 16,tempVal);
	if(status != GT_OK)
	{
		osPrintfErr("PHY1340 link force up phase 1-3 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,16,tempVal);
		return status;
	}
	/* set
	  *  Reg 0
	  *  bit[11]:0: Normal operation
	  */
	status = smiReadRegister(devNum, portNum, 0, &tempVal);
	if (status != GT_OK)
	{
		/* HW problem or PHY not connected to a port */
		osPrintfErr("PHY1340 link force up phase 1-2 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,0,tempVal);
		return status;
	}
	tempVal &= 0xF7FF;
	status = smiWriteRegister(devNum,portNum, 0,tempVal);
	if(status != GT_OK)
	{
		osPrintfErr("PHY1340 link force up phase 1-3 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,0,tempVal);
		return status;
	}
	/* set
	  *  Reg 16
	  *  bit[2]:0: Normal operation
	  */
	status = smiReadRegister(devNum, portNum, 16, &tempVal);
	if (status != GT_OK)
	{
		/* HW problem or PHY not connected to a port */
		osPrintfErr("PHY1340 link force up phase 1-2 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,16,tempVal);
		return status;
	}
	tempVal &= 0xFFFB;
	status = smiWriteRegister(devNum,portNum, 16,tempVal);
	if(status != GT_OK)
	{
		osPrintfErr("PHY1340 link force up phase 1-3 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,16,tempVal);
		return status;
	}
	return status;
}

/*******************************************************************************
* phy1240LinkForceDown
*
* DESCRIPTION:
*   This function implements the force link down operation on phy port type 88E1240
*
* INPUTS:
*       devNum    - Device Number.
*       portNum   - Port Number.
*		enable 	  - Set or Clear operation.
*		smiReadRegister - Phy port SMI read function.
*		smiWriteRegister - Phy port SMI write function.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS phy1240LinkForceDown
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    IN GT_U8 enable,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_READ_FUNC  smiReadRegister,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
)
{
	GT_STATUS 	status; /* return value */
	GT_U16		tempVal;/* Value to set to the register */

	/* Copper Specific Control register 0
	  *  bit[11]:1: Power Down
	  *
	  * write register 22 value 0:select page 0
	  */
	/* phase 1 step 1 */
	tempVal = 0;
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
	if(status != GT_OK)
	{		
		osPrintf("PHY1240 link force down phase 1-1 select page write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
		return status;
	}
	/* set
	  *  Reg 0
	  *  bit[11]:1: Power down
	  */
	status = smiReadRegister(devNum, portNum, 0, &tempVal);
	if (status != GT_OK)
	{
		/* HW problem or PHY not connected to a port */
		osPrintf("PHY1240 link force down phase 1-2 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,0,tempVal);
		return status;
	}
	if(enable) { /* set link force down */
		tempVal |= (1<<11);
	}
	else { /* clear link force down */
		#if 0
		/*set restart Auto-Negotiation*/
		status = smiReadRegister(devNum, portNum, 0, &tempVal);
		if (status != GT_OK){
			/* HW problem or PHY not connected to a port */
			osPrintf("PHY1240 link force down phase 1-2 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,0,tempVal);
			return status;
	    }
		tempVal &= 0xFDFF;
		status = smiWriteRegister(devNum,portNum, 0,tempVal);
		if(status != GT_OK){
			osPrintf("PHY1240 link force down phase 1-3 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,0,tempVal);
			return status;
		}
		/*set software reset*/
		status = smiReadRegister(devNum, portNum, 0, &tempVal);
		if (status != GT_OK){
			/* HW problem or PHY not connected to a port */
			osPrintf("PHY1240 link force down phase 1-2 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,0,tempVal);
			return status;
	    }
		tempVal &= 0x7FFF;
		status = smiWriteRegister(devNum,portNum, 0,tempVal);
		if(status != GT_OK){
			osPrintf("PHY1240 link force down phase 1-3 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,0,tempVal);
			return status;
		}
		#endif
		tempVal &= 0xF7FF;
	}
	status = smiWriteRegister(devNum,portNum, 0,tempVal);
	if(status != GT_OK)
	{
		osPrintf("PHY1240 link force down phase 1-3 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,0,tempVal);
		return status;
	}
    /* set
	  *  Reg 16
	  *  bit[2]:1: Power down
	  */
	status = smiReadRegister(devNum, portNum, 16, &tempVal);
	if (status != GT_OK)
	{
		/* HW problem or PHY not connected to a port */
		osPrintf("PHY1240 link force down phase 1-2 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,16,tempVal);
		return status;
	}
	if(enable) { /* set link force down */
		tempVal |= (1<<2);
	}
	else { /* clear link force down */
		#if 0
		/*set restart Auto-Negotiation*/
		status = smiReadRegister(devNum, portNum, 0, &tempVal);
		if (status != GT_OK){
			/* HW problem or PHY not connected to a port */
			osPrintf("PHY1240 link force down phase 1-2 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,0,tempVal);
			return status;
	    }
		tempVal &= 0xFDFF;
		status = smiWriteRegister(devNum,portNum, 0,tempVal);
		if(status != GT_OK){
			osPrintf("PHY1240 link force down phase 1-3 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,0,tempVal);
			return status;
		}
		/*set software reset*/
		status = smiReadRegister(devNum, portNum, 0, &tempVal);
		if (status != GT_OK){
			/* HW problem or PHY not connected to a port */
			osPrintf("PHY1240 link force down phase 1-2 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,0,tempVal);
			return status;
	    }
		tempVal &= 0x7FFF;
		status = smiWriteRegister(devNum,portNum, 0,tempVal);
		if(status != GT_OK){
			osPrintf("PHY1240 link force down phase 1-3 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,0,tempVal);
			return status;
		}
		#endif
		tempVal &= 0xFFFB;
	}
	status = smiWriteRegister(devNum,portNum, 16,tempVal);
	if(status != GT_OK)
	{
		osPrintf("PHY1240 link force down phase 1-3 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,16,tempVal);
		return status;
	}
	return status;
}
/*******************************************************************************
* phy1340LinkForceDown
*
* DESCRIPTION:
*   This function implements the force link down operation on phy port type 88E1240
*
* INPUTS:
*       devNum    - Device Number.
*       portNum   - Port Number.
*		enable 	  - Set or Clear operation.
*		smiReadRegister - Phy port SMI read function.
*		smiWriteRegister - Phy port SMI write function.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS phy1340LinkForceDown
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    IN GT_U8 enable,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_READ_FUNC  smiReadRegister,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
)
{
	GT_STATUS 	status; /* return value */
	GT_U16		tempVal;/* Value to set to the register */

	/* Copper Specific Control register 0
	  *  bit[11]:1: Power Down
	  *
	  * write register 22 value 0:select page 0
	  */
	/* phase 1 step 1 */
    #if 1
	/* do not write page to 0,for copper & fiber is different */
	status = smiReadRegister(devNum, portNum, PRV_CPSS_GE_PHY_EXT_ADDR, &tempVal);
	if (status != GT_OK)
	{
		/* Read the current phy page */
		osPrintfErr("PHY1340 Read page :dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
		return status;
	}
	osPrintfDbg("PHY1340 Read page :dev %d port %d reg %d val %#0x OK\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
    #else
	tempVal = 0;
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
	if(status != GT_OK)
	{		
		osPrintfErr("PHY1340 link force down phase 1-1 select page write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
		return status;
	}
    #endif	
	
	/* set
	  *  Reg 0
	  *  bit[11]:1: Power down
	  */
	status = smiReadRegister(devNum, portNum, 0, &tempVal);
	if (status != GT_OK)
	{
		/* HW problem or PHY not connected to a port */
		osPrintfErr("PHY1340 link force down phase 1-2 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,0,tempVal);
		return status;
	}
	if(enable) { /* set link force down */
		tempVal |= (1<<11);
	}
	else { /* clear link force down */
		#if 0
		/*set restart Auto-Negotiation*/
		status = smiReadRegister(devNum, portNum, 0, &tempVal);
		if (status != GT_OK){
			/* HW problem or PHY not connected to a port */
			osPrintf("PHY1240 link force down phase 1-2 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,0,tempVal);
			return status;
	    }
		tempVal &= 0xFDFF;
		status = smiWriteRegister(devNum,portNum, 0,tempVal);
		if(status != GT_OK){
			osPrintf("PHY1240 link force down phase 1-3 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,0,tempVal);
			return status;
		}
		/*set software reset*/
		status = smiReadRegister(devNum, portNum, 0, &tempVal);
		if (status != GT_OK){
			/* HW problem or PHY not connected to a port */
			osPrintf("PHY1240 link force down phase 1-2 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,0,tempVal);
			return status;
	    }
		tempVal &= 0x7FFF;
		status = smiWriteRegister(devNum,portNum, 0,tempVal);
		if(status != GT_OK){
			osPrintf("PHY1240 link force down phase 1-3 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,0,tempVal);
			return status;
		}
		#endif
		tempVal &= 0xF7FF;
	}
	status = smiWriteRegister(devNum,portNum, 0,tempVal);
	if(status != GT_OK)
	{
		osPrintfErr("PHY1340 link force down phase 1-3 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,0,tempVal);
		return status;
	}
    /* set
	  *  Reg 16
	  *  bit[2]:1: Power down
	  */
	status = smiReadRegister(devNum, portNum, 16, &tempVal);
	if (status != GT_OK)
	{
		/* HW problem or PHY not connected to a port */
		osPrintfErr("PHY1340 link force down phase 1-2 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,16,tempVal);
		return status;
	}
	if(enable) { /* set link force down */
		tempVal |= (1<<2);
	}
	else { /* clear link force down */
		#if 0
		/*set restart Auto-Negotiation*/
		status = smiReadRegister(devNum, portNum, 0, &tempVal);
		if (status != GT_OK){
			/* HW problem or PHY not connected to a port */
			osPrintf("PHY1240 link force down phase 1-2 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,0,tempVal);
			return status;
	    }
		tempVal &= 0xFDFF;
		status = smiWriteRegister(devNum,portNum, 0,tempVal);
		if(status != GT_OK){
			osPrintf("PHY1240 link force down phase 1-3 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,0,tempVal);
			return status;
		}
		/*set software reset*/
		status = smiReadRegister(devNum, portNum, 0, &tempVal);
		if (status != GT_OK){
			/* HW problem or PHY not connected to a port */
			osPrintf("PHY1240 link force down phase 1-2 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,0,tempVal);
			return status;
	    }
		tempVal &= 0x7FFF;
		status = smiWriteRegister(devNum,portNum, 0,tempVal);
		if(status != GT_OK){
			osPrintf("PHY1240 link force down phase 1-3 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,0,tempVal);
			return status;
		}
		#endif
		tempVal &= 0xFFFB;
	}
	status = smiWriteRegister(devNum,portNum, 16,tempVal);
	if(status != GT_OK)
	{
		osPrintfErr("PHY1340 link force down phase 1-3 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,16,tempVal);
		return status;
	}
	return status;
}
/*******************************************************************************
* phy1112AutoState
*
* DESCRIPTION:
*   This function implements auto-negotiation operation on phy port type 88E1240
*
* INPUTS:
*       devNum    - Device Number.
*       portNum   - Port Number.
*		enable 	  - Set or Clear operation.(0 - disable AN, 1 - enable AN)
*		smiReadRegister - Phy port SMI read function.
*		smiWriteRegister - Phy port SMI write function.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS phy1112AutoState
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    IN GT_U8 enable,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_READ_FUNC  smiReadRegister,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
)
{
	GT_STATUS 	status = 0; /* return value */
	GT_U16		tempVal = 0, old_page = 0;/* Value to set to the register */

	/*
	 * Disable PHY Auto-Negotiation
	 */
	PRV_CPSS_PP_MAC(devNum)->phyInfo.genVctBindFunc.cpssPhyAutoNegotiationSet( \
					devNum, portNum, GT_FALSE);
	/* backup old page value */
	status = smiReadRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,&old_page);
	if(GT_OK != status) {
		osPrintfErr("phy 1112 backup page read PHY:dev %d port %d reg %d val %#0x error %d\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,old_page,status);
		goto restore_page;
	}
	
	/* Copper Specific Control register 0
	  *
	  * write register 22 value 0:select page 0
	  */
	/* phase 1 step 1 */
	tempVal = 0;
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
	if(status != GT_OK)
	{		
		osPrintf("PHY1112 auto state phase 1-1 select page write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
		goto restore_page;
	}
	/*
	 *  set register 0
	 *  bit[15]:1: Software PHY reset
	 *  bit[12]:1: Enable/Disable Auto-Negotiation Process
	 */
	status = smiReadRegister(devNum, portNum, 0, &tempVal);
	if(status != GT_OK){
		osPrintf("PHY1112 auto state phase 1-2 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);		
		goto restore_page;
	}
	tempVal |= (1<<15); /* bit[15]:1 */
	if(0 == enable){ /* disable auto-nego */
		tempVal &= 0xEFFF;
	}
	else { /* enable auto-nego */
		tempVal |= (1<<12);
	}	
	status = smiWriteRegister(devNum,portNum, 0,tempVal);
	if(status != GT_OK){
		osPrintf("PHY1112 auto state phase 1-3 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);
		goto restore_page;
	}
restore_page:	
	/* restore old page selection */
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,old_page);
	if(GT_OK != status) {
		osPrintfErr("phy 1112 restore page write PHY:dev %d port %d reg %d data %#0x error %d\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,old_page,status);
		return status;
	}
	/*
	 * Enable PHY Auto-Negotiation
	 */
	PRV_CPSS_PP_MAC(devNum)->phyInfo.genVctBindFunc.cpssPhyAutoNegotiationSet( \
					devNum, portNum, GT_TRUE);
	return status;
}

/*******************************************************************************
* phy1240AutoState
*
* DESCRIPTION:
*   This function implements auto-negotiation operation on phy port type 88E1240
*
* INPUTS:
*       devNum    - Device Number.
*       portNum   - Port Number.
*		enable 	  - Set or Clear operation.(0 - disable AN, 1 - enable AN)
*		smiReadRegister - Phy port SMI read function.
*		smiWriteRegister - Phy port SMI write function.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS phy1240AutoState
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    IN GT_U8 enable,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_READ_FUNC  smiReadRegister,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
)
{
	GT_STATUS 	status; /* return value */
	GT_U16		tempVal;/* Value to set to the register */

	/* Copper Specific Control register 0
	  *  bit[11]:1: Power Down
	  *
	  * write register 22 value 0:select page 0
	  */
	/* phase 1 step 1 */
	tempVal = 0;
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
	if(status != GT_OK)
	{		
		osPrintf("PHY1240 auto state phase 1-1 select page write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
		return status;
	}

	status = smiReadRegister(devNum, portNum, 0, &tempVal);
	if (status != GT_OK){
		/* HW problem or PHY not connected to a port */
		osPrintf("PHY1240 auto state phase 1-2 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);
		return status;
    }
	/*
	 * Power Down
	 */
	tempVal |= (1<<11);
	status = smiWriteRegister(devNum,portNum, 0,tempVal);
	if(status != GT_OK){
		return status;
	}
	 /*
	  *  set register 0
	  *  bit[15]:1: Software PHY reset
	  *  bit[12]:1: Enable/Disable Auto-Negotiation Process
	  */
	status = smiReadRegister(devNum, portNum, 0, &tempVal);
	if(status != GT_OK){
		osPrintf("PHY1240 auto state phase 1-3 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);		
		return status;
	}
	tempVal |= (1<<15); /* bit[15]:1 */
	if(0 == enable){ /* disable auto-nego */
		tempVal &= 0xEFFF;
	}
	else { /* enable auto-nego */
		tempVal |= (1<<12);
	}	
	status = smiWriteRegister(devNum,portNum, 0,tempVal);
	if(status != GT_OK){
		osPrintf("PHY1240 auto state phase 1-3 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);
		return status;
	}
	/* 
	 * No Power Down
	 */
	status = smiReadRegister(devNum, portNum, 0, &tempVal);
	if (status != GT_OK){
		/* HW problem or PHY not connected to a port */
		osPrintf("PHY1240 auto state phase 1-4 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);
		return status;
    }
	tempVal &= 0xF7FF;
	status = smiWriteRegister(devNum,portNum, 0,tempVal);
	if(status != GT_OK){
		osPrintf("PHY1240 auto state phase 1-4 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);
		return status;
	}
	return status;
}

/*******************************************************************************
* phy1340AutoState
*
* DESCRIPTION:
*   This function implements auto-negotiation operation on phy port type 88E1240
*
* INPUTS:
*       devNum    - Device Number.
*       portNum   - Port Number.
*		enable 	  - Set or Clear operation.(0 - disable AN, 1 - enable AN)
*		smiReadRegister - Phy port SMI read function.
*		smiWriteRegister - Phy port SMI write function.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS phy1340AutoState
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    IN GT_U8 enable,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_READ_FUNC  smiReadRegister,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
)
{
	GT_STATUS 	status; /* return value */
	GT_U16		tempVal;/* Value to set to the register */

	/* Copper Specific Control register 0
	  *  bit[11]:1: Power Down
	  *
	  * write register 22 value 0:select page 0
	  */
	/* phase 1 step 1 */
    #if 1
	/* do not write page to 0,for copper & fiber is different */
	status = smiReadRegister(devNum, portNum, PRV_CPSS_GE_PHY_EXT_ADDR, &tempVal);
	if (status != GT_OK)
	{
		/* Read the current phy page */
		osPrintfErr("PHY1340 Read page :dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
		return status;
	}
	osPrintfDbg("PHY1340 Read page :dev %d port %d reg %d val %#0x OK\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
    #else	
	tempVal = 0;
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
	if(status != GT_OK)
	{		
		osPrintf("PHY1340 auto state phase 1-1 select page write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
		return status;
	}
    #endif
	status = smiReadRegister(devNum, portNum, 0, &tempVal);
	if (status != GT_OK){
		/* HW problem or PHY not connected to a port */
		osPrintfErr("PHY1340 auto state phase 1-2 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);
		return status;
    }
	/*
	 * Power Down
	 */
	tempVal |= (1<<11);
	status = smiWriteRegister(devNum,portNum, 0,tempVal);
	if(status != GT_OK){
		return status;
	}
	 /*
	  *  set register 0
	  *  bit[15]:1: Software PHY reset
	  *  bit[12]:1: Enable/Disable Auto-Negotiation Process
	  */
	status = smiReadRegister(devNum, portNum, 0, &tempVal);
	if(status != GT_OK){
		osPrintfErr("PHY1340 auto state phase 1-3 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);		
		return status;
	}
	tempVal |= (1<<15); /* bit[15]:1 */
	if(0 == enable){ /* disable auto-nego */
		tempVal &= 0xEFFF;
	}
	else { /* enable auto-nego */
		tempVal |= (1<<12);
	}	
	status = smiWriteRegister(devNum,portNum, 0,tempVal);
	if(status != GT_OK){
		osPrintfErr("PHY1340 auto state phase 1-3 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);
		return status;
	}
	/* 
	 * No Power Down
	 */
	status = smiReadRegister(devNum, portNum, 0, &tempVal);
	if (status != GT_OK){
		/* HW problem or PHY not connected to a port */
		osPrintfErr("PHY1340 auto state phase 1-4 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);
		return status;
    }
	tempVal &= 0xF7FF;
	status = smiWriteRegister(devNum,portNum, 0,tempVal);
	if(status != GT_OK){
		osPrintfErr("PHY1340 auto state phase 1-4 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);
		return status;
	}
	return status;
}


/*******************************************************************************
* phy114xAutoState
*
* DESCRIPTION:
*   This function implements auto-negotiation operation on phy port type 88E114x
*
* INPUTS:
*       devNum    - Device Number.
*       portNum   - Port Number.
*		enable 	  - Set or Clear operation.(0 - disable AN, 1 - enable AN)
*		smiReadRegister - Phy port SMI read function.
*		smiWriteRegister - Phy port SMI write function.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS phy114xAutoState
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    IN GT_U8 enable,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_READ_FUNC  smiReadRegister,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
)
{
	GT_STATUS 	status; /* return value */
	GT_U16		tempVal;/* Value to set to the register */

	/* Copper Specific Control register 0
	  *  bit[11]:1: Power Down
	  *
	  * write register 22 value 0:select page 0
	  */
	/* phase 1 step 1 */
	tempVal = 0;
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
	if(status != GT_OK)
	{		
		osPrintf("PHY114x auto state phase 1-1 select page write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
		return status;
	}

	status = smiReadRegister(devNum, portNum, 0, &tempVal);
	if (status != GT_OK){
		/* HW problem or PHY not connected to a port */
		osPrintf("PHY114x auto state phase 1-2 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);
		return status;
    }
	/*
	 * Power Down
	 */
	tempVal |= (1<<11);
	status = smiWriteRegister(devNum,portNum, 0,tempVal);
	if(status != GT_OK){
		return status;
	}
	 /*
	  *  set register 0
	  *  bit[15]:1: Software PHY reset
	  *  bit[12]:1: Enable/Disable Auto-Negotiation Process
	  */
	status = smiReadRegister(devNum, portNum, 0, &tempVal);
	if(status != GT_OK){
		osPrintf("PHY114x auto state phase 1-3 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);		
		return status;
	}
	tempVal |= (1<<15); /* bit[15]:1 */
	if(0 == enable){ /* disable auto-nego */
		tempVal &= 0xEFFF;
	}
	else { /* enable auto-nego */
		tempVal |= (1<<12);
	}	
	status = smiWriteRegister(devNum,portNum, 0,tempVal);
	if(status != GT_OK){
		osPrintf("PHY114x auto state phase 1-3 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);
		return status;
	}
	/* 
	 * No Power Down
	 */
	status = smiReadRegister(devNum, portNum, 0, &tempVal);
	if (status != GT_OK){
		/* HW problem or PHY not connected to a port */
		osPrintf("PHY114x auto state phase 1-4 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);
		return status;
    }
	tempVal &= 0xF7FF;
	status = smiWriteRegister(devNum,portNum, 0,tempVal);
	if(status != GT_OK){
		osPrintf("PHY114x auto state phase 1-4 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);
		return status;
	}
	return status;
}

/*******************************************************************************
* phy1112SpeedSet
*
* DESCRIPTION:
*   This function implements speed_set operation on phy port type 88E1112
*
* INPUTS:
*       devNum    - Device Number.
*       portNum   - Port Number.
*		enable 	  - (0 - speed 10, 1 - speed 100, 2 - speed 1000)
*		smiReadRegister - Phy port SMI read function.
*		smiWriteRegister - Phy port SMI write function.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS phy1112SpeedSet
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    IN GT_U8 enable, /*0::speed 10  1::speed 100  2::speed 1000*/
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_READ_FUNC  smiReadRegister,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
)
{
	GT_STATUS 	status = 0; /* return value */
	GT_U16		tempVal = 0, old_page = 0;/* Value to set to the register */

	/*
	 * Disable PHY Auto-Negotiation
	 */
	PRV_CPSS_PP_MAC(devNum)->phyInfo.genVctBindFunc.cpssPhyAutoNegotiationSet( \
					devNum, portNum, GT_FALSE);
	/* backup old page value */
	status = smiReadRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,&old_page);
	if(GT_OK != status) {
		osPrintfErr("phy 1112 backup page read PHY:dev %d port %d reg %d val %#0x error %d\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,old_page,status);
		goto restore_page;
	}
	
	/* Copper Specific Control register 0
	  *
	  * write register 22 value 0:select page 0
	  */
	/* phase 1 step 1 */
	tempVal = 0;
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
	if(status != GT_OK)
	{		
		osPrintf("PHY1112 speed set phase 1-1 select page write PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
		goto restore_page;
	}
	/*
	 *  set register 0
	 *  bit[15]:1: Software PHY reset
	 *	bit[6],bit[13]:  0,0 - speed 10;
	 *					 0,1 - speed 100;
	 *					 1,0 - speed 1000
	 */
	status = smiReadRegister(devNum, portNum, 0, &tempVal);
	if(status != GT_OK){
		osPrintf("PHY1112 speed set phase 1-2 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);		
		goto restore_page;
	}
	tempVal |= (1<<15);     /* Phy reset bit[15]:1 */
	if(0 == enable){        /*** set speed 10 ***/
		tempVal &= 0xFFBF;  /*bit[6]:0*/
		tempVal &= 0xDFFF;  /*bit[13]:0*/
	}
	else if(1 == enable){   /*** set speed 100 ***/
		tempVal &= 0xFFBF;  /*bit[6]:0*/
		tempVal |= (1<<13); /*bit[13]:1*/
	}
	else if(2 == enable){   /*** set speed 1000 ***/
		tempVal |= (1<<6);  /*bit[6]:1*/
		tempVal &= 0xDFFF;  /*bit[13]:0*/
	}
	status = smiWriteRegister(devNum,portNum, 0,tempVal);
	if(status != GT_OK){
		osPrintf("PHY1112 speed set phase 1-3 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);
		goto restore_page;
	}
restore_page:	
	/* restore old page selection */
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,old_page);
	if(GT_OK != status) {
		osPrintfErr("phy 1112 restore page write PHY:dev %d port %d reg %d data %#0x error %d\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,old_page,status);
		return status;
	}
	/*
	 * Enable PHY Auto-Negotiation
	 */
	PRV_CPSS_PP_MAC(devNum)->phyInfo.genVctBindFunc.cpssPhyAutoNegotiationSet( \
					devNum, portNum, GT_TRUE);
	return status;
}

/*******************************************************************************
* phy1240SpeedSet
*
* DESCRIPTION:
*   This function implements speed_set operation on phy port type 88E1240
*
* INPUTS:
*       devNum    - Device Number.
*       portNum   - Port Number.
*		enable 	  - (0 - speed 10, 1 - speed 100, 2 - speed 1000)
*		smiReadRegister - Phy port SMI read function.
*		smiWriteRegister - Phy port SMI write function.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS phy1240SpeedSet
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    IN GT_U8 enable, /*0::speed 10  1::speed 100  2::speed 1000*/
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_READ_FUNC  smiReadRegister,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
)
{
	GT_STATUS 	status; /* return value */
	GT_U16		tempVal;/* Value to set to the register */

	/* Copper Specific Control register 0
	  *  bit[11]:1: Power Down
	  *
	  * write register 22 value 0:select page 0
	  */
	/* phase 1 step 1 */
	tempVal = 0;
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
	if(status != GT_OK)
	{		
		osPrintf("PHY1240 speed set phase 1-1 select page write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
		return status;
	}
	/*
	 * Power Down
	 */
	status = smiReadRegister(devNum, portNum, 0, &tempVal);
	if (status != GT_OK){
		/* HW problem or PHY not connected to a port */
		osPrintf("PHY1240 speed set phase 1-2 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);
		return status;
    }
	tempVal |= (1<<11); /*bit[11]:1*/
	status = smiWriteRegister(devNum,portNum, 0,tempVal);
	if(status != GT_OK){
		osPrintf("PHY1240 speed set phase 1-2 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);		
		return status;
	}
	 /*
	  *  set register 0
	  *  bit[15]:1: Software PHY reset
	  *  bit[6],bit[13]:  0,0 - speed 10;
	  * 				  0,1 - speed 100;
	  *                   1,0 - speed 1000
	  */
	status = smiReadRegister(devNum, portNum, 0, &tempVal);
	if(status != GT_OK){
		osPrintf("PHY1240 speed set phase 1-3 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);		
		return status;
	}
	tempVal |= (1<<15);    /* bit[15]:1 */
	if(0 == enable){       /*** set speed 10 ***/
		tempVal &= 0xFFBF; /*bit[6]:0*/ 
		tempVal &= 0xDFFF; /*bit[13]:0*/ 
	}
	else if(1 == enable){  /*** set speed 100 ***/
		tempVal &= 0xFFBF; /*bit[6]:0*/
		tempVal |= (1<<13);/*bit[13]:1*/
	}
	else if(2 == enable){  /*** set speed 1000 ***/
		tempVal |= (1<<6); /*bit[6]:1*/
		tempVal &= 0xDFFF; /*bit[13]:0*/
	}
	status = smiWriteRegister(devNum,portNum, 0,tempVal);
	if(status != GT_OK){
		osPrintf("PHY1240 speed set phase 1-3 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);
		return status;
	}
	/* 
	 * No Power Down
	 */
	status = smiReadRegister(devNum, portNum, 0, &tempVal);
	if (status != GT_OK){
		/* HW problem or PHY not connected to a port */
		osPrintf("PHY1240 speed set phase 1-4 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);
		return status;
    }
	tempVal &= 0xF7FF;  /*bit[11]:0*/
	status = smiWriteRegister(devNum,portNum, 0,tempVal);
	if(status != GT_OK){
		osPrintf("PHY1240 speed set phase 1-4 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);
		return status;
	}
	return status;
}

/*******************************************************************************
* phy1340SpeedSet
*
* DESCRIPTION:
*   This function implements speed_set operation on phy port type 88E1240
*
* INPUTS:
*       devNum    - Device Number.
*       portNum   - Port Number.
*		enable 	  - (0 - speed 10, 1 - speed 100, 2 - speed 1000)
*		smiReadRegister - Phy port SMI read function.
*		smiWriteRegister - Phy port SMI write function.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS phy1340SpeedSet
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    IN GT_U8 enable, /*0::speed 10  1::speed 100  2::speed 1000*/
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_READ_FUNC  smiReadRegister,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
)
{
	GT_STATUS 	status; /* return value */
	GT_U16		tempVal;/* Value to set to the register */

	/* Copper Specific Control register 0
	  *  bit[11]:1: Power Down
	  *
	  * write register 22 value 0:select page 0
	  */
	/* phase 1 step 1 */
    #if 1
	/* do not write page to 0,for copper & fiber is different */
	status = smiReadRegister(devNum, portNum, PRV_CPSS_GE_PHY_EXT_ADDR, &tempVal);
	if (status != GT_OK)
	{
		/* Read the current phy page */
		osPrintfErr("PHY1340 Read page :dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
		return status;
	}
	osPrintfDbg("PHY1340 Read page :dev %d port %d reg %d val %#0x OK\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
    #else	
		
	tempVal = 0;
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
	if(status != GT_OK)
	{		
		osPrintf("PHY1340 speed set phase 1-1 select page write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
		return status;
	}
	#endif
	/*
	 * Power Down
	 */
	status = smiReadRegister(devNum, portNum, 0, &tempVal);
	if (status != GT_OK){
		/* HW problem or PHY not connected to a port */
		osPrintfErr("PHY1340 speed set phase 1-2 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);
		return status;
    }
	tempVal |= (1<<11); /*bit[11]:1*/
	status = smiWriteRegister(devNum,portNum, 0,tempVal);
	if(status != GT_OK){
		osPrintfErr("PHY1340 speed set phase 1-2 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);		
		return status;
	}
	 /*
	  *  set register 0
	  *  bit[15]:1: Software PHY reset
	  *  bit[6],bit[13]:  0,0 - speed 10;
	  * 				  0,1 - speed 100;
	  *                   1,0 - speed 1000
	  */
	status = smiReadRegister(devNum, portNum, 0, &tempVal);
	if(status != GT_OK){
		osPrintfErr("PHY1340 speed set phase 1-3 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);		
		return status;
	}
	tempVal |= (1<<15);    /* bit[15]:1 */
	if(0 == enable){       /*** set speed 10 ***/
		tempVal &= 0xFFBF; /*bit[6]:0*/ 
		tempVal &= 0xDFFF; /*bit[13]:0*/ 
	}
	else if(1 == enable){  /*** set speed 100 ***/
		tempVal &= 0xFFBF; /*bit[6]:0*/
		tempVal |= (1<<13);/*bit[13]:1*/
	}
	else if(2 == enable){  /*** set speed 1000 ***/
		tempVal |= (1<<6); /*bit[6]:1*/
		tempVal &= 0xDFFF; /*bit[13]:0*/
	}
	status = smiWriteRegister(devNum,portNum, 0,tempVal);
	if(status != GT_OK){
		osPrintfErr("PHY1340 speed set phase 1-3 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);
		return status;
	}
	/* 
	 * No Power Down
	 */
	status = smiReadRegister(devNum, portNum, 0, &tempVal);
	if (status != GT_OK){
		/* HW problem or PHY not connected to a port */
		osPrintfErr("PHY1340 speed set phase 1-4 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);
		return status;
    }
	tempVal &= 0xF7FF;  /*bit[11]:0*/
	status = smiWriteRegister(devNum,portNum, 0,tempVal);
	if(status != GT_OK){
		osPrintfErr("PHY1340 speed set phase 1-4 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);
		return status;
	}
	return status;
}

/*******************************************************************************
* phy114xSpeedSet
*
* DESCRIPTION:
*   This function implements speed_set operation on phy port type 88E114x
*
* INPUTS:
*       devNum    - Device Number.
*       portNum   - Port Number.
*		enable 	  - (0 - speed 10, 1 - speed 100, 2 - speed 1000)
*		smiReadRegister - Phy port SMI read function.
*		smiWriteRegister - Phy port SMI write function.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS phy114xSpeedSet
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    IN GT_U8 enable, /*0::speed 10  1::speed 100  2::speed 1000*/
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_READ_FUNC  smiReadRegister,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
)
{
	GT_STATUS 	status; /* return value */
	GT_U16		tempVal;/* Value to set to the register */

	/* Copper Specific Control register 0
	  *  bit[11]:1: Power Down
	  *
	  * write register 22 value 0:select page 0
	  */
	/* phase 1 step 1 */
	tempVal = 0;
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
	if(status != GT_OK)
	{		
		osPrintf("PHY114x speed set phase 1-1 select page write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
		return status;
	}
	/*
	 * Power Down
	 */
	status = smiReadRegister(devNum, portNum, 0, &tempVal);
	if (status != GT_OK){
		/* HW problem or PHY not connected to a port */
		osPrintf("PHY114x speed set phase 1-2 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);
		return status;
    }
	tempVal |= (1<<11); /*bit[11]:1*/
	status = smiWriteRegister(devNum,portNum, 0,tempVal);
	if(status != GT_OK){
		osPrintf("PHY114x speed set phase 1-2 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);		
		return status;
	}
	 /*
	  *  set register 0
	  *  bit[15]:1: Software PHY reset
	  *  bit[6],bit[13]:  0,0 - speed 10;
	  * 				  0,1 - speed 100;
	  *                   1,0 - speed 1000
	  */
	status = smiReadRegister(devNum, portNum, 0, &tempVal);
	if(status != GT_OK){
		osPrintf("PHY114x speed set phase 1-3 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);		
		return status;
	}
	tempVal |= (1<<15);    /* bit[15]:1 */
	if(0 == enable){       /*** set speed 10 ***/
		tempVal &= 0xFFBF; /*bit[6]:0*/ 
		tempVal &= 0xDFFF; /*bit[13]:0*/ 
	}
	else if(1 == enable){  /*** set speed 100 ***/
		tempVal &= 0xFFBF; /*bit[6]:0*/
		tempVal |= (1<<13);/*bit[13]:1*/
	}
	else if(2 == enable){  /*** set speed 1000 ***/
		tempVal |= (1<<6); /*bit[6]:1*/
		tempVal &= 0xDFFF; /*bit[13]:0*/
	}
	status = smiWriteRegister(devNum,portNum, 0,tempVal);
	if(status != GT_OK){
		osPrintf("PHY114x speed set phase 1-3 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);
		return status;
	}
	/* 
	 * No Power Down
	 */
	status = smiReadRegister(devNum, portNum, 0, &tempVal);
	if (status != GT_OK){
		/* HW problem or PHY not connected to a port */
		osPrintf("PHY114x speed set phase 1-4 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);
		return status;
    }
	tempVal &= 0xF7FF;  /*bit[11]:0*/
	status = smiWriteRegister(devNum,portNum, 0,tempVal);
	if(status != GT_OK){
		osPrintf("PHY114x speed set phase 1-4 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);
		return status;
	}
	return status;
}

/*******************************************************************************
* phy1112DuplexSet
*
* DESCRIPTION:
*   This function implements duplex_set operation on phy port type 88E1112
*
* INPUTS:
*       devNum    - Device Number.
*       portNum   - Port Number.
*		enable 	  - (1 - full, 0 - half)
*		smiReadRegister - Phy port SMI read function.
*		smiWriteRegister - Phy port SMI write function.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS phy1112DuplexSet
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    IN GT_U8 enable, /*1 - full, 0 - half*/
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_READ_FUNC  smiReadRegister,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
)
{
	GT_STATUS	status = 0; /* return value */
	GT_U16		tempVal = 0, old_page = 0;/* Value to set to the register */

	/*
	 * Disable PHY Auto-Negotiation
	 */
	PRV_CPSS_PP_MAC(devNum)->phyInfo.genVctBindFunc.cpssPhyAutoNegotiationSet( \
					devNum, portNum, GT_FALSE);
	/* backup old page value */
	status = smiReadRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,&old_page);
	if(GT_OK != status) {
		osPrintfErr("phy 1112 backup page read PHY:dev %d port %d reg %d val %#0x error %d\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,old_page,status);
		goto restore_page;
	}
	
	/* Copper Specific Control register 0
	  * 
	  * write register 22 value 0:select page 0
	  */
	/* phase 1 step 1 */
	tempVal = 0;
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
	if(status != GT_OK)
	{		
		osPrintf("PHY1112 duplex mode phase 1-1 select page write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
		goto restore_page;
	}
	 /*
	  *  set register 0
	  *  bit[15]:1: Software PHY reset
	  *  bit[8]:1: set full/half copper duplex mode Process
	  */
	status = smiReadRegister(devNum, portNum, 0, &tempVal);
	if(status != GT_OK){
		osPrintf("PHY1112 duplex mode phase 1-2 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);		
		goto restore_page;
	}
	tempVal |= (1<<15); /* bit[15]:1 */
	if(0 == enable){ /* half */
		tempVal &= 0xFEFF;
	}
	else { /* full */
		tempVal |= (1<<8);
	}	
	status = smiWriteRegister(devNum,portNum, 0,tempVal);
	if(status != GT_OK){
		osPrintf("PHY1112 duplex mode phase 1-3 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);
		goto restore_page;
	}
restore_page:	
	/* restore old page selection */
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,old_page);
	if(GT_OK != status) {
		osPrintfErr("phy 1112 restore page write PHY:dev %d port %d reg %d data %#0x error %d\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,old_page,status);
		return status;
	}
	/*
	 * Enable PHY Auto-Negotiation
	 */
	PRV_CPSS_PP_MAC(devNum)->phyInfo.genVctBindFunc.cpssPhyAutoNegotiationSet( \
					devNum, portNum, GT_TRUE);
	return status;
}


/*******************************************************************************
* phy1240DuplexSet
*
* DESCRIPTION:
*   This function implements duplex_set operation on phy port type 88E1240
*
* INPUTS:
*       devNum    - Device Number.
*       portNum   - Port Number.
*		enable 	  - (1 - full, 0 - half)
*		smiReadRegister - Phy port SMI read function.
*		smiWriteRegister - Phy port SMI write function.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS phy1240DuplexSet
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    IN GT_U8 enable, /*1 - full, 0 - half*/
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_READ_FUNC  smiReadRegister,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
)
{
	GT_STATUS 	status; /* return value */
	GT_U16		tempVal;/* Value to set to the register */

	/* Copper Specific Control register 0
	  * 
	  * write register 22 value 0:select page 0
	  */
	/* phase 1 step 1 */
	tempVal = 0;
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
	if(status != GT_OK)
	{		
		osPrintf("PHY1240 duplex mode phase 1-1 select page write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
		return status;
	}
	 /*
	  *  set register 0
	  *  bit[15]:1: Software PHY reset
	  *  bit[8]:1: set full/half copper duplex mode Process
	  */
	status = smiReadRegister(devNum, portNum, 0, &tempVal);
	if(status != GT_OK){
		osPrintf("PHY1240 duplex mode phase 1-3 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);		
		return status;
	}
	tempVal |= (1<<15); /* bit[15]:1 */
	if(0 == enable){ /* half */
		tempVal &= 0xFEFF;
	}
	else { /* full */
		tempVal |= (1<<8);
	}	
	status = smiWriteRegister(devNum,portNum, 0,tempVal);
	if(status != GT_OK){
		osPrintf("PHY1240 duplex mode phase 1-3 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);
		return status;
	}
	return status;
}

/*******************************************************************************
* phy1340DuplexSet
*
* DESCRIPTION:
*   This function implements duplex_set operation on phy port type 88E1240
*
* INPUTS:
*       devNum    - Device Number.
*       portNum   - Port Number.
*		enable 	  - (1 - full, 0 - half)
*		smiReadRegister - Phy port SMI read function.
*		smiWriteRegister - Phy port SMI write function.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS phy1340DuplexSet
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    IN GT_U8 enable, /*1 - full, 0 - half*/
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_READ_FUNC  smiReadRegister,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
)
{
	GT_STATUS 	status; /* return value */
	GT_U16		tempVal;/* Value to set to the register */

	/* Copper Specific Control register 0
	  * 
	  * write register 22 value 0:select page 0
	  */
	/* phase 1 step 1 */
    #if 1
	/* do not write page to 0,for copper & fiber is different */
	status = smiReadRegister(devNum, portNum, PRV_CPSS_GE_PHY_EXT_ADDR, &tempVal);
	if (status != GT_OK)
	{
		/* Read the current phy page */
		osPrintfErr("PHY1340 Read page :dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
		return status;
	}
	osPrintfDbg("PHY1340 Read page :dev %d port %d reg %d val %#0x OK\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
    #else	
	
	tempVal = 0;
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
	if(status != GT_OK)
	{		
		osPrintf("PHY1340 duplex mode phase 1-1 select page write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
		return status;
	}
	#endif
	 /*
	  *  set register 0
	  *  bit[15]:1: Software PHY reset
	  *  bit[8]:1: set full/half copper duplex mode Process
	  */
	status = smiReadRegister(devNum, portNum, 0, &tempVal);
	if(status != GT_OK){
		osPrintfErr("PHY1340 duplex mode phase 1-3 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);		
		return status;
	}
	tempVal |= (1<<15); /* bit[15]:1 */
	if(0 == enable){ /* half */
		tempVal &= 0xFEFF;
	}
	else { /* full */
		tempVal |= (1<<8);
	}	
	status = smiWriteRegister(devNum,portNum, 0,tempVal);
	if(status != GT_OK){
		osPrintfErr("PHY1340 duplex mode phase 1-3 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);
		return status;
	}
	return status;
}

/*******************************************************************************
* phy114xDuplexSet
*
* DESCRIPTION:
*   This function implements duplex_set operation on phy port type 88E114x
*
* INPUTS:
*       devNum    - Device Number.
*       portNum   - Port Number.
*		enable 	  - (1 - full, 0 - half)
*		smiReadRegister - Phy port SMI read function.
*		smiWriteRegister - Phy port SMI write function.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS phy114xDuplexSet
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    IN GT_U8 enable, /*1 - full, 0 - half*/
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_READ_FUNC  smiReadRegister,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
)
{
	GT_STATUS	status; /* return value */
	GT_U16		tempVal;/* Value to set to the register */

	/* Copper Specific Control register 0
	  * 
	  * write register 22 value 0:select page 0
	  */
	/* phase 1 step 1 */
	tempVal = 0;
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
	if(status != GT_OK)
	{		
		osPrintf("PHY114x duplex mode phase 1-1 select page write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
		return status;
	}
	 /*
	  *  set register 0
	  *  bit[15]:1: Software PHY reset
	  *  bit[8]:1: set full/half copper duplex mode Process
	  */
	status = smiReadRegister(devNum, portNum, 0, &tempVal);
	if(status != GT_OK){
		osPrintf("PHY114x duplex mode phase 1-3 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);		
		return status;
	}
	tempVal |= (1<<15); /* bit[15]:1 */
	if(0 == enable){ /* half */
		tempVal &= 0xFEFF;
	}
	else { /* full */
		tempVal |= (1<<8);
	}	
	status = smiWriteRegister(devNum,portNum, 0,tempVal);
	if(status != GT_OK){
		osPrintf("PHY114x duplex mode phase 1-3 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);
		return status;
	}
	return status;
}

/*******************************************************************************
* phy1112RebootSet
*
* DESCRIPTION:
*   This function implements reboot_set operation on phy port type 88E1112
*
* INPUTS:
*       devNum    - Device Number.
*       portNum   - Port Number.
*		smiReadRegister - Phy port SMI read function.
*		smiWriteRegister - Phy port SMI write function.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS phy1112RebootSet
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_READ_FUNC  smiReadRegister,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
)
{
	GT_STATUS	status = 0; /* return value */
	GT_U16		tempVal = 0, old_page = 0;/* Value to set to the register */

	/*
	 * Disable PHY Auto-Negotiation
	 */
	PRV_CPSS_PP_MAC(devNum)->phyInfo.genVctBindFunc.cpssPhyAutoNegotiationSet( \
					devNum, portNum, GT_FALSE);
	/* backup old page value */
	status = smiReadRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,&old_page);
	if(GT_OK != status) {
		osPrintfErr("phy 1112 backup page read PHY:dev %d port %d reg %d val %#0x error %d\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,old_page,status);
		goto restore_page;
	}

	/* Copper Specific Control register 0
	  * 
	  * write register 22 value 0:select page 0
	  */
	/* phase 1 step 1 */
	tempVal = 0;
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
	if(status != GT_OK)
	{		
		osPrintf("PHY1112 reboot phase 1-1 select page write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
		goto restore_page;
	}
	 /*
	  *  set register 0
	  *  bit[15]:1: Software PHY reset
	  */
	status = smiReadRegister(devNum, portNum, 0, &tempVal);
	if(status != GT_OK){
		osPrintf("PHY1112 reboot phase 1-2 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);		
		goto restore_page;
	}
	tempVal |= (1<<15); /* bit[15]:1 */	
	status = smiWriteRegister(devNum,portNum, 0,tempVal);
	if(status != GT_OK){
		osPrintf("PHY1112 reboot phase 1-3 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);
		goto restore_page;
	}
restore_page:	
	/* restore old page selection */
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,old_page);
	if(GT_OK != status) {
		osPrintfErr("phy 1112 restore page write PHY:dev %d port %d reg %d data %#0x error %d\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,old_page,status);
		return status;
	}
	/*
	 * Enable PHY Auto-Negotiation
	 */
	PRV_CPSS_PP_MAC(devNum)->phyInfo.genVctBindFunc.cpssPhyAutoNegotiationSet( \
					devNum, portNum, GT_TRUE);
	return status;
}


/*******************************************************************************
* phy1240RebootSet
*
* DESCRIPTION:
*   This function implements reboot_set operation on phy port type 88E1240
*
* INPUTS:
*       devNum    - Device Number.
*       portNum   - Port Number.
*		smiReadRegister - Phy port SMI read function.
*		smiWriteRegister - Phy port SMI write function.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS phy1240RebootSet
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_READ_FUNC  smiReadRegister,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
)
{
	GT_STATUS 	status; /* return value */
	GT_U16		tempVal;/* Value to set to the register */

	/* Copper Specific Control register 0
	  * 
	  * write register 22 value 0:select page 0
	  */
	/* phase 1 step 1 */
	tempVal = 0;
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
	if(status != GT_OK)
	{		
		osPrintf("PHY1240 reboot phase 1-1 select page write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
		return status;
	}
	 /*
	  *  set register 0
	  *  bit[15]:1: Software PHY reset
	  */
	status = smiReadRegister(devNum, portNum, 0, &tempVal);
	if(status != GT_OK){
		osPrintf("PHY1240 reboot phase 1-3 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);		
		return status;
	}
	tempVal |= (1<<15); /* bit[15]:1 */	
	status = smiWriteRegister(devNum,portNum, 0,tempVal);
	if(status != GT_OK){
		osPrintf("PHY1240 reboot phase 1-3 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);
		return status;
	}
	return status;
}


/*******************************************************************************
* phy1340RebootSet
*
* DESCRIPTION:
*   This function implements reboot_set operation on phy port type 88E1240
*
* INPUTS:
*       devNum    - Device Number.
*       portNum   - Port Number.
*		smiReadRegister - Phy port SMI read function.
*		smiWriteRegister - Phy port SMI write function.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS phy1340RebootSet
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_READ_FUNC  smiReadRegister,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
)
{
	GT_STATUS 	status; /* return value */
	GT_U16		tempVal;/* Value to set to the register */

	/* Copper Specific Control register 0
	  * 
	  * write register 22 value 0:select page 0
	  */
	/* phase 1 step 1 */
    #if 1
	/* do not write page to 0,for copper & fiber is different */
	status = smiReadRegister(devNum, portNum, PRV_CPSS_GE_PHY_EXT_ADDR, &tempVal);
	if (status != GT_OK)
	{
		/* Read the current phy page */
		osPrintfErr("PHY1340 Read page :dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
		return status;
	}
	osPrintfDbg("PHY1340 Read page :dev %d port %d reg %d val %#0x OK\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
    #else		
	tempVal = 0;
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
	if(status != GT_OK)
	{		
		osPrintf("PHY1340 reboot phase 1-1 select page write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
		return status;
	}
	#endif
	 /*
	  *  set register 0
	  *  bit[15]:1: Software PHY reset
	  */
	status = smiReadRegister(devNum, portNum, 0, &tempVal);
	if(status != GT_OK){
		osPrintfErr("PHY1340 reboot phase 1-3 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);		
		return status;
	}
	tempVal |= (1<<15); /* bit[15]:1 */	
	status = smiWriteRegister(devNum,portNum, 0,tempVal);
	if(status != GT_OK){
		osPrintfErr("PHY1340 reboot phase 1-3 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);
		return status;
	}
	return status;
}


/*******************************************************************************
* phy114xRebootSet
*
* DESCRIPTION:
*   This function implements reboot_set operation on phy port type 88E114x
*
* INPUTS:
*       devNum    - Device Number.
*       portNum   - Port Number.
*		smiReadRegister - Phy port SMI read function.
*		smiWriteRegister - Phy port SMI write function.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS phy114xRebootSet
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_READ_FUNC  smiReadRegister,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
)
{
	GT_STATUS	status; /* return value */
	GT_U16		tempVal;/* Value to set to the register */

	/* Copper Specific Control register 0
	  * 
	  * write register 22 value 0:select page 0
	  */
	/* phase 1 step 1 */
	tempVal = 0;
	status = smiWriteRegister(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
	if(status != GT_OK)
	{		
		osPrintf("PHY114x reboot phase 1-1 select page write PHY:dev %d port %d reg %d val %#0x error\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,tempVal);
		return status;
	}
	 /*
	  *  set register 0
	  *  bit[15]:1: Software PHY reset
	  */
	status = smiReadRegister(devNum, portNum, 0, &tempVal);
	if(status != GT_OK){
		osPrintf("PHY114x reboot phase 1-3 read PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);		
		return status;
	}
	tempVal |= (1<<15); /* bit[15]:1 */	
	status = smiWriteRegister(devNum,portNum, 0,tempVal);
	if(status != GT_OK){
		osPrintf("PHY114x reboot phase 1-3 write PHY:dev %d port %d reg %d val %#0x error\r\n", \
				devNum,portNum,0,tempVal);
		return status;
	}
	return status;
}
#endif


/*******************************************************************************
* cpssGenPhyTypeGet
*
* DESCRIPTION:
*   Gets the PHY type.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - Device Number.
*       portNum         - Port Number.
*       smiReadRegister - Read function
*
* OUTPUTS:
*       isMarvellPhyPtr - GT_TRUE - phy type is Marvell
*                         GT_FALSE - phy type is not Marvell
*       phyTypePtr      - PHY type
*
* RETURNS :
*       GT_OK   - on success or current phy is not 88E1149.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS phyTypeGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    OUT GT_BOOL     *isMarvellPhyPtr,
    OUT GT_U16      *phyTypePtr,
    IN  CPSS_GEN_PHY_PORT_SMI_REGISTER_READ_FUNC  smiReadRegister
)
{
    GT_STATUS status;

    *isMarvellPhyPtr = GT_FALSE;

    /* read Marvel OUI */
    status = smiReadRegister(devNum, portNum, PRV_CPSS_PHY_ID0, phyTypePtr);
    if (status != GT_OK)
    {
        /* HW problem or PHY not connected to a port */
        return status;
    }

    if (*phyTypePtr == PRV_CPSS_MARVELL_OUI_MSB)
    {
        *isMarvellPhyPtr = GT_TRUE;
    }
    else
    {
        /* it's not a Marvell PHY */
        return GT_OK;
    }

    /* read model number and revision */
    status = smiReadRegister(devNum, portNum, PRV_CPSS_PHY_ID1, phyTypePtr);

    return status;
}
/*******************************************************************************
* cpssGenPhyTypeGet
*
* DESCRIPTION:
*   Gets the PHY type.
*
* INPUTS:
*       devNum    - Device Number.
*       portNum   - Port Number.
*
* OUTPUTS:
*	  isMarvellPhyPtr - PHY OUI is for Marvell
*	  phyTypePtr - PHY type
*
* RETURNS :
*		GT_OK	- on success or current phy is not 88E1149.
*		GT_FAIL - otherwise.
*
* COMMENTS:
*           use by npd - cpss1.3
*
*******************************************************************************/
GT_STATUS cpssGenPhyTypeGet
(
	IN	GT_U8		devNum,
	IN	GT_U8		portNum,
	OUT GT_BOOL 	*isMarvellPhyPtr,
	OUT GT_U16		*phyTypePtr,
	IN	CPSS_GEN_PHY_PORT_SMI_REGISTER_READ_FUNC  smiReadRegister
)
{
	GT_STATUS result = GT_OK;
	GT_BOOL isMarvellPhy = GT_FALSE;
	GT_U16 phyType = 0;

	result = phyTypeGet(devNum,portNum,&isMarvellPhy,&phyType,smiReadRegister);
	
	*isMarvellPhyPtr = isMarvellPhy;
	*phyTypePtr = phyType;

	return result;
}


/*******************************************************************************
* prvCpssGenPhyErrataInitFix
*
* DESCRIPTION:
*   This function implements the WA for phys during the initialization.
*
* APPLICABLE DEVICES:  
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - Device Number.
*       portNum             - Port Number.
*       smiReadRegister     - Read function
*       smiWriteRegister    - Write function
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssGenPhyErrataInitFix
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_READ_FUNC  smiReadRegister,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
)
{
    GT_U16 phyType;           /* PHY type */
    GT_BOOL isMarvellPhy;     /* indicator whether the PHY is a Marvell PHY */
    GT_STATUS status;         /* returned status */

    if(0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        return GT_BAD_PARAM;
    }
    PRV_CPSS_PHY_PORT_CHECK_MAC(devNum,portNum);

    /* No errata to fix on XG port */
    if (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType >= PRV_CPSS_PORT_XG_E)
    {
        return GT_OK;
    }

    /* get PHY type. Don't check return status. It may be not GT_OK if PHY
       not connected to a port */
    phyTypeGet(devNum, portNum, &isMarvellPhy, &phyType, smiReadRegister);

    /* Fix errata for Marvell PHYs */
    if (isMarvellPhy)
    {
        switch(phyType & PRV_CPSS_PHY_MODEL_MASK)
        {
            case PRV_CPSS_DEV_E1112: /* for PHY 1112 fix the SERDES lock-up erratum */
                                     if ((status = phyLockUpprevent(devNum,
                                                                    portNum,
                                                                    smiReadRegister,
                                                                    smiWriteRegister)) != GT_OK)
                                     {
                                         return status;
                                     }
                                     
                                     #ifndef __AX_PLATFORM__
                                     /* set relevant Auto-MediaSelectEn bit in PHY Auto-Negotiation Configuration Register0 */
                                     if((status = cpssDxChPhySmiAutoMediaSelectSet(devNum,
                                                                     portNum,
                                                                     (1<<(portNum%6))))!= GT_OK)
                                     {
                                         osPrintf("prvCpssGenPhyErrataInitFix::PhyAutoMediaSelectSet dev %d port %d error %d\n", \
                                                         devNum,portNum,status);
                                         return status;
                                     }
                                     
                                     if((status = phy1112ConfigFix(devNum,
                                                                     portNum,
                                                                     smiReadRegister,
                                                                     smiWriteRegister)) != GT_OK)
                                     {
                                         osPrintf("prvCpssGenPhyErrataInitFix::phy1112ConfigFix error %d\n",status);
                                         return status;
                                     }
                                     #endif
                                     break;

            case PRV_CPSS_DEV_E114X: /* for PHY 114X erratum */
                                     #ifdef __AX_PLATFORM__
                                     osPrintf("prvCpssGenPhyErrataInitFix::MV phyType %#0x is 114X\r\n",phyType);
                                     if ((status = phy114xErratafix (devNum, portNum)) != GT_OK)
                                     {
                                         return status;
                                     }
                                     #else
                                     if ((status = phy114xErratafix (devNum, portNum,smiReadRegister,smiWriteRegister)) != GT_OK)
                                     {
                                         return status;
                                     }
                                     #endif
                                     break;

            case PRV_CPSS_DEV_E1149: if ((phyType & PRV_CPSS_PHY_REV_MASK) >= 4)
                                     {
                                        /* for PHY 1149 Rev. C1 fix the gigabit VOD erratum */
                                        if ((status = phy1149GigabitVodSet(devNum,
                                                                           portNum,
                                                                           0x7777,
                                                                           smiWriteRegister)) != GT_OK)
                                        {
                                            return status;
                                        }

                                        /* set enumerator(in bitmap) for phy 1149 */
                                        PRV_CPSS_PHY_ERRATA_SET(devNum,
                                                                portNum,
                                                                PRV_CPSS_PHY_ERRATA_1149_REG_9_WRITE_WA_E);

                                     }
                                     break;

            case PRV_CPSS_DEV_E1240: /* for PHY 1240 config */
                                     #ifndef __AX_PLATFORM__
                                     if ((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1240)
                                     {
                                         if((status = phy1240ConfigFix(devNum,
                                                                         portNum,
                                                                         smiReadRegister,
                                                                         smiWriteRegister)) != GT_OK)
                                         {
                                             osPrintf("prvCpssGenPhyErrataInitFix::phy1240ConfigFix error %d\n",status);
                                             return status;
                                         }
                                     }
                                     #endif
                                     break;

            case PRV_CPSS_DEV_E1340_X0:
            case PRV_CPSS_DEV_E1340: /* WA for the 88E1340 */
                                     if ((status = phy1340Init(devNum,
                                                               portNum,
                                                               smiWriteRegister)) != GT_OK)
                                     {
                                         return status;
                                     }
                                     break;

            default: break;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssPhyRegWriteErrataFix
*
* DESCRIPTION:
*   Implement PHY register write workarounds.
*
* APPLICABLE DEVICES:  
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - Device Number.
*       portNum   - Port Number.
*       regPhy    - phy's register address
*       data      - value to be set
*       smiWriteRegister - pointer to relevant SMI write implementation
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssPhyRegWriteErrataFix
(
    IN GT_U8  devNum,
    IN GT_U8  portNum,
    IN GT_U8  regPhy,
    IN GT_U16 data,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
)
{

    GT_U32    testMode; /* test mode: 0 - Normal Mode
                                      1 - Transmit waveform test
                                      2 - Transmit Jitter Test, master mode
                                      3 - Transmit Jitter Test, slave mode
                                      4 - Transmit Distortion test */
    GT_STATUS rc;       /* return code */
    GT_U16    tmpData;  /* temporary data*/

    if((PRV_CPSS_PHY_ERRATA_GET(devNum,
                               portNum,
                          PRV_CPSS_PHY_ERRATA_1149_REG_9_WRITE_WA_E) == GT_TRUE)
    {
        /* WA is relevant for register 9 only */
        if(regPhy == 9)
        {

            /* check if Test mode - bits 13:15 being set*/
            testMode = (data >> 13);

            switch(testMode)
            {
                case 0:
                    /* Set Vod to 14% */
                    tmpData = 0x7777;
                    break;
                case 1:
                case 2:
                case 3:
                case 4:
                    /* Set Vod to 0% */
                    tmpData = 0x0;
                    break;
                default:
                    /* Set Vod to 14% as for normal mode */
                    tmpData = 0x7777;
                    break;
            }/* of switch */

            if ((rc = phy1149GigabitVodSet(devNum,
                                               portNum,
                                               tmpData,
                                               smiWriteRegister)) != GT_OK)
            {
                return rc;
            }
        }/* register 9*/
    }

    return GT_OK ;
}

/*******************************************************************************
* prvCpssFixPhy1240SgmiiLockUpErrata
*
* DESCRIPTION:
*   This function implements the WA for the 88E1240 SGMII lock-up issue per 
*   port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - Device Number.
*       portNum   - Port Number.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on access to phy other then 1240.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssFixPhy1240SgmiiLockUpErrata
(
    IN GT_U8 devNum,
    IN GT_U8 portNum
)
{
    GT_STATUS rc;               /* returned status */
    GT_U16    regData;          /* write to register */
    GT_U16    phyType;          /* PHY type */
    GT_BOOL   isMarvellPhy;     /* indicator whether the PHY is a Marvell PHY */
    GT_U32    i;                /* loop iterator */

    /* get PHY type. Don't check return status. It may be not GT_OK if PHY */
    /* not connected to a port */
    phyTypeGet(devNum, portNum, &isMarvellPhy, &phyType, 
               (PRV_CPSS_PP_VCT_BIND_FUNC_MAC(devNum, portNum)->genVctBindFunc.cpssPhyRegisterRead));

    if( (isMarvellPhy == GT_FALSE)  || 
        ((phyType & PRV_CPSS_PHY_MODEL_MASK) != PRV_CPSS_DEV_E1240) )
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }

    /* 5 iteration of WA algorithm */
    for (i = 0; i < 5; i++)
    {
        /* set Page2 */
        rc = PRV_PHY_REG_WRITE_MAC(devNum, portNum, PRV_CPSS_GE_PHY_EXT_ADDR, 0x2);
        if(rc != GT_OK)
            return rc;

        /* read register 17 on page 2 */
        rc = PRV_PHY_REG_READ_MAC(devNum, portNum, PRV_CPSS_PHY_SPEC_STATUS, &regData);
        if(rc != GT_OK)
            return rc;

        /* check 17.10 bit - SGMII link status and 17.6 bit - SGMII sync status*/
        if ((regData & BIT_10) && (regData & BIT_6))
        {
            /* no need to perform WA */
            /* restore future PHY access to page 0 */
            rc = PRV_PHY_REG_WRITE_MAC(devNum, portNum, PRV_CPSS_GE_PHY_EXT_ADDR, 0);
            if(rc != GT_OK)
                return rc;
            break;
        }
        else
        {   
            /* Perform WA */

            /* Power down the PHY */
            /* set Page0 */
            rc = PRV_PHY_REG_WRITE_MAC(devNum, portNum, PRV_CPSS_GE_PHY_EXT_ADDR, 0);
            if(rc != GT_OK)
                return rc;
    
            /* Write Register 0 */
            rc = PRV_PHY_REG_WRITE_MAC(devNum, portNum, PRV_CPSS_PHY_CNTRL, 0x1940);
            if(rc != GT_OK)
                return rc;
    
            /* Reset PCS digital logic */
            /* Write Register 29 */
            rc = PRV_PHY_REG_WRITE_MAC(devNum, portNum, PRV_CPSS_PHY_REG29, 0x001D);
            if(rc != GT_OK)
                return rc;
    
            /* Write Register 30 */
            rc = PRV_PHY_REG_WRITE_MAC(devNum, portNum, PRV_CPSS_PHY_REG30, 0xEF00);
            if(rc != GT_OK)
                return rc;
    
            /* Reset TBG */
            /* Write Register 29 */
            rc = PRV_PHY_REG_WRITE_MAC(devNum, portNum, PRV_CPSS_PHY_REG29, 0x0017);
            if(rc != GT_OK)
                return rc;
    
            /* Write Register 30 */
            rc = PRV_PHY_REG_WRITE_MAC(devNum, portNum, PRV_CPSS_PHY_REG30, 0x8300);
            if(rc != GT_OK)
                return rc;
    
            /* Write Register 30 */
            rc = PRV_PHY_REG_WRITE_MAC(devNum, portNum, PRV_CPSS_PHY_REG30, 0x8100);
            if(rc != GT_OK)
                return rc;
    
            /* Write Register 30 */
            rc = PRV_PHY_REG_WRITE_MAC(devNum, portNum, PRV_CPSS_PHY_REG30, 0x0000);
            if(rc != GT_OK)
                return rc;
    
            rc = cpssOsTimerWkAfter(5);
            if ( rc != GT_OK )
            {
                return (rc);
            }
            
            /* De-assert PCS reset */
            /* Write Register 29 */
            rc = PRV_PHY_REG_WRITE_MAC(devNum, portNum, PRV_CPSS_PHY_REG29, 0x001D);
            if(rc != GT_OK)
                return rc;
    
            /* Write Register 30 */
            rc = PRV_PHY_REG_WRITE_MAC(devNum, portNum, PRV_CPSS_PHY_REG30, 0x0000);
            if(rc != GT_OK)
                return rc;
    
            /* Release PHY port from Power down mode */
            /* Write register 0 */
            rc = PRV_PHY_REG_WRITE_MAC(devNum, portNum, PRV_CPSS_PHY_CNTRL, 0x9140);
            if(rc != GT_OK)
                return rc;
    
            rc = cpssOsTimerWkAfter(100);
            if ( rc != GT_OK )
            {
                return (rc);
            }
        }
    }
    
    return GT_OK;
}

/*******************************************************************************
* prvCpssFixPhy1340BgaInitErrata
*
* DESCRIPTION:
*   This function implements the WA for the 88E1340 BGA package init sequence
*   per port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - Device Number.
*       portNum   - Port Number.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on access to phy other then 1340.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssFixPhy1340BgaInitErrata
(
    IN GT_U8 devNum,
    IN GT_U8 portNum
)
{
    GT_STATUS rc;               /* returned status */
    GT_U16    phyType;          /* PHY type */
    GT_BOOL   isMarvellPhy;     /* indicator whether the PHY is a Marvell PHY */
    
    /* get PHY type. Don't check return status. It may be not GT_OK if PHY */
    /* not connected to a port */
    phyTypeGet(devNum, portNum, &isMarvellPhy, &phyType, 
               (PRV_CPSS_PP_VCT_BIND_FUNC_MAC(devNum, portNum)->genVctBindFunc.cpssPhyRegisterRead));

    if( (isMarvellPhy == GT_FALSE)  || 
        (((phyType & PRV_CPSS_PHY_MODEL_MASK) != PRV_CPSS_DEV_E1340_X0) &&
         ((phyType & PRV_CPSS_PHY_MODEL_MASK) != PRV_CPSS_DEV_E1340)) )
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }
    
    /* set Page0 */
    rc = PRV_PHY_REG_WRITE_MAC(devNum, portNum, PRV_CPSS_GE_PHY_EXT_ADDR, 0x0);
    if(rc != GT_OK)
        return rc;

    /* Write Register 29 */
    rc = PRV_PHY_REG_WRITE_MAC(devNum, portNum, PRV_CPSS_PHY_REG29, 0x0003);
    if(rc != GT_OK)
        return rc;
    
    /* Write Register 30 */
    rc = PRV_PHY_REG_WRITE_MAC(devNum, portNum, PRV_CPSS_PHY_REG30, 0x0002);
    if(rc != GT_OK)
        return rc;
    
    /* Write Register 29 */
    rc = PRV_PHY_REG_WRITE_MAC(devNum, portNum, PRV_CPSS_PHY_REG29, 0x0);
    return rc;
}

/*******************************************************************************
* cpssPhyErrataWaExecute
*
* DESCRIPTION:
*   This function execute workaround for PHY errata from a selected list.
*
* APPLICABLE DEVICES:  
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - Device Number.
*       portNum     - Port Number.
*       errataWa    - the selected workaround to execute
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - on not initialized function pointers for
*                                  read and write phy registers operations.
*       GT_BAD_PARAM             - on wrong device, port or requested WA
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssPhyErrataWaExecute
(
   IN GT_U8                     devNum,
   IN GT_U8                     portNum,
   IN CPSS_PHY_ERRATA_WA_ENT    errataWa
)
{
    GT_STATUS   status;         /* returned status */

    if( (PRV_CPSS_IS_DEV_EXISTS_MAC(devNum) == 0) ||
        (!PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, portNum)) )
    {
        return GT_BAD_PARAM;
    }

    /* check read and write functions were bound */
    if (( (PRV_CPSS_PP_VCT_BIND_FUNC_MAC(devNum, port)->
                                         genVctBindFunc.cpssPhyRegisterRead) ==  NULL ) ||
        ( (PRV_CPSS_PP_VCT_BIND_FUNC_MAC(devNum, port)->
                                         genVctBindFunc.cpssPhyRegisterWrite) ==  NULL ))
    {
        return GT_NOT_INITIALIZED;
    }
    
    switch(errataWa)
    {
        case CPSS_PHY_ERRATA_WA_88E1240_SGMII_LOCKUP_E:
            status = prvCpssFixPhy1240SgmiiLockUpErrata(devNum, portNum);
            break;
        case CPSS_PHY_ERRATA_WA_88E1340_BGA_INIT_E:
            status = prvCpssFixPhy1340BgaInitErrata(devNum, portNum);
            break;
        default:
            return GT_BAD_PARAM;
    }
    
    return status;
}


#ifndef __AX_PLATFORM__
extern GT_STATUS cpssDxChPhyPortSmiRegisterRead
(
	IN	GT_U8	  devNum,
	IN	GT_U8	  portNum,
	IN	GT_U8	  phyReg,
	OUT GT_U16	  *dataPtr
);

extern GT_STATUS cpssDxChPhyPortSmiRegisterWrite
(
	IN	GT_U8	  devNum,
	IN	GT_U8	  portNum,
	IN	GT_U8	  phyReg,
	IN	GT_U16	  data
);

/*******************************************************************************
* cpssDxChPhyPortReadTest
*
* DESCRIPTION:
*   test procedure for port PHY register read
*
* INPUTS:
*       devNum    - Device Number.
*       portNum   - Port Number.
*       phyReg    - phy's register address
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
******************************************************************************/
GT_STATUS cpssDxChPhyPortReadTest
(
	IN GT_U8  devNum,
	IN GT_U8  portNum,
	IN GT_U8  phyReg
)
{
	GT_STATUS rc = GT_OK;
	GT_U16 readData = 0;

	printf("%-8s%-6s%-5s%-6s\n","device","port","phy","data");
	printf("%-8s%-6s%-5s%-6s\n","------","----","---","----");

	rc = cpssDxChPhyPortSmiRegisterRead(devNum,portNum,phyReg,&readData);
	printf("  %-7d%-5d%-7d%-6x\n",devNum,portNum,phyReg,readData);
	
	return GT_OK;
}

/*******************************************************************************
* cpssDxChPhyPortWriteTest
*
* DESCRIPTION:
*   test procedure for port PHY register write
*
* INPUTS:
*       devNum    - Device Number.
*       portNum   - Port Number.
*       phyReg    - phy's register address
*	  data 	   - data to write
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
******************************************************************************/
GT_STATUS cpssDxChPhyPortWriteTest
(
	IN GT_U8  devNum,
	IN GT_U8  portNum,
	IN GT_U8  phyReg,
	IN GT_U16 data
)
{
	GT_STATUS rc = GT_OK;
	GT_U16 readData = 0;

	printf("%-8s%-6s%-5s%-6s%-8s\n","device","port","phy","data","result");
	printf("%-8s%-6s%-5s%-6s%-8s\n","------","----","---","----","------");

	rc = cpssDxChPhyPortSmiRegisterWrite(devNum,portNum,phyReg,data);
	printf("  %-7d%-5d%-7d%-6x",devNum,portNum,phyReg,data);
	if(GT_OK == rc) {
		rc = cpssDxChPhyPortSmiRegisterRead(devNum,portNum,phyReg,&readData);
		printf("%-8s",(data == readData) ? "ok":"err");
	}
	else {
		printf("bad");
	}
	printf("\n");
	
	return GT_OK;
}

/*******************************************************************************
* cpssDxChPhyPortCheck
*
* DESCRIPTION:
*   test procedure for port PHY register read
*
* INPUTS:
*       devNum    - Device Number.
*       portNum   - Port Number.
*       regPhy    - phy's register address
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
******************************************************************************/
GT_STATUS cpssDxChPhyPortCheck
(
	IN GT_U8  devNum,
	IN GT_U8  portNum
)
{
	GT_STATUS rc = GT_OK;
    GT_U16 phyType;           /* PHY type */
    GT_BOOL isMarvellPhy;     /* indicator whether the PHY is a Marvell PHY */
	
    if(0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        return GT_BAD_PARAM;
    }
    PRV_CPSS_PHY_PORT_CHECK_MAC(devNum,portNum);

    /* No errata to fix on XG port */
    if (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray->portType == PRV_CPSS_PORT_XG_E)
    {
        return GT_OK;
    }

    /* get PHY type. Don't check return status. It may be not GT_OK if PHY
       not connected to a port */
    rc = phyTypeGet(devNum, portNum, &isMarvellPhy, &phyType, &cpssDxChPhyPortSmiRegisterRead);
	printf("  %-8d%-4d%-8s%-10s\n",devNum,portNum,isMarvellPhy ? "marvell":"-", \
			((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1112) ? "88E1112":
			((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E114X) ? "88E114x":
			((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1149) ? "88E1149":
			((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1240) ? "88E1240":"-");
	
	return rc;
}

/*******************************************************************************
* cpssDxChPhyPortAllCheck
*
* DESCRIPTION:
*   test procedure for all port PHY register read
*
* INPUTS:
*	  None.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
******************************************************************************/
GT_STATUS cpssDxChPhyPortAllCheck
(
	GT_VOID
)
{
	GT_STATUS rc = GT_OK;
	GT_U8 devNum = 0;
	GT_U8 portNum = 0;

	printf("%-8s%-6s%-8s%-10s\n","device","port","vendor","identity");
	printf("%-8s%-6s%-8s%-10s\n","------","----","------","--------");

	for(portNum = 0;portNum <= 23; portNum++) {
		rc = cpssDxChPhyPortCheck(devNum,portNum);
	}
	
	return rc;
}
#endif

