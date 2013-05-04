/*******************************************************************************
Copyright (C) Autelan Technology

This software file is owned and distributed by Autelan Technology 
********************************************************************************

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
* npd_board.c
*
*
* CREATOR:
*		luoxun@autelan.com
*
* DESCRIPTION:
*		Hardware diagnosis routines.
*
* DATE:
*		07/07/2011	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.1 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

#include "npd_log.h"
#include "npd_hw_diag.h"

int prbs_get_slotnum(unsigned char lionNum, unsigned char groupId, unsigned char portNum)
{
    unsigned char slotNum = 0;
    
    if((lionNum > 1) || (groupId > 3) || (portNum > 24))
        return 0;
    
    if(0 == lionNum)
    {
        switch(groupId)
        {
            case 0:
                if(portNum < 12)
                    slotNum = 1;
                else
                    slotNum = 10;
                break;
            case 1:
                if(portNum < 12)
                    slotNum = 9;
                else
                    slotNum = 2;
                break;
            case 2:
                if(portNum < 12)
                    slotNum = 3;
                else
                    slotNum = 8;
                break;
            case 3:
                if(portNum < 12)
                    slotNum = 7;
                else
                    slotNum = 4;
                break;
        }
    }
    else if(1 == lionNum)
    {
        switch(groupId)
        {
            case 0:
                if(portNum < 12)
                    slotNum = 4;
                else
                    slotNum = 7;
                break;
            case 1:
                if(portNum < 12)
                    slotNum = 8;
                else
                    slotNum = 3;
                break;
            case 2:
                if(portNum < 12)
                    slotNum = 2;
                else
                    slotNum = 9;
                break;
            case 3:
                if(portNum < 8)    /* Special for Lion1-Group3 */
                    slotNum = 10;
                else
                    slotNum = 1;
                break;
        }
    }

    return slotNum;
}

int npd_hw_diag_ax8610_prbs_test(unsigned char lionNum, unsigned int *pSlotBitMap)
{
    int ret = RET_SUCCESS;
    unsigned char i = 0;
    unsigned char portCount = 24; /* Count for one Group */
    unsigned char GroupId   = 0;
    unsigned char devNum    = 0;
    unsigned char slotNum   = 0;
    unsigned int prbs_type_regAddr   = 0x09800394;
    unsigned int prbs_enable_regAddr = 0x09800378;
    unsigned int prbs_status_regAddr = 0x0980037c;
    unsigned int regAddr  = 0x0;
    unsigned int regValue = 0x0;
    /* 
        * bitx <-> slotx
        * If slotx status error, bitx will be set to 1.
        * bit1 <-> slot1
        * ......
        * bit10 <-> slot10 
        */
    unsigned int slotBitMap = 0x0;  

    /* 
        * Set the type of PRBS
        *   PRBS7  : 0x80
        *   PRBS15: 0x82
        */
    for(GroupId=0; GroupId<4; GroupId++)
    {
        regAddr = prbs_type_regAddr;
        for(i=0; i<portCount; i++, regAddr+=0x400)
        {
            ret = nam_write_reg(devNum, GroupId, regAddr, 0x82);
            RET_CHECK;
        }
    }

    /* 
        * Enable PRBS
        */
    for(GroupId=0; GroupId<4; GroupId++)
    {
        regAddr = prbs_enable_regAddr;
        for(i=0; i<portCount; i++, regAddr+=0x400)
        {
            ret = nam_write_reg(devNum, GroupId, regAddr, 0x800e);
            RET_CHECK;
        }
    }

    /* 
        * Check the status of PRBS
        */
    for(GroupId=0; GroupId<4; GroupId++)
    {
        regAddr = prbs_status_regAddr;
        /* Special for Lion1-Group3 */
        if((1 == lionNum) && (3 == GroupId))
            portCount = 20;
        
        for(i=0; i<portCount; i++, regAddr+=0x400)
        {
            ret = nam_read_reg(devNum, GroupId, regAddr, &regValue);
            RET_CHECK;
            if( (regValue & 0xc000) != 0xc000 )
            {
                npd_syslog_err("STATUS ERROR: GroupId=%d  portNum=%d, regAddr=%x, regValue=%x\n", 
                    GroupId, i, regAddr, regValue);
                slotNum = prbs_get_slotnum(lionNum, GroupId, i);
                npd_syslog_dbg("      slotNum=%d\n", slotNum);
                npd_syslog_dbg("      slotBitMap before modification=0x%x\n", slotBitMap);
                slotBitMap = slotBitMap | (1<<slotNum);
                npd_syslog_dbg("      slotBitMap after  modification=0x%x\n", slotBitMap);
            }
        }
    }

    *pSlotBitMap = slotBitMap;
    npd_syslog_event("PRBS setting end. slotBitMap = 0x%x\n", slotBitMap);
    
    return RET_SUCCESS;

}



#ifdef __cplusplus
}
#endif 
