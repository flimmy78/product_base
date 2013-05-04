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
 ***************************************************************************/
#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif

#include <linux/module.h>
#include <linux/kernel.h>

#if CONFIG_MODEREVISION
#define MODREVISION
#include <linux/modeversions.h>
#endif

#include "sysdep.h"
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
#include <linux/fs.h>
#include <linux/net.h>
#include <linux/mm.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/tqueue.h>

#include "include/driverRegDefs.h"



/* external function decleretions */
extern void enable_irq(int irq_num);
extern void disable_irq(int irq_num);

/* forword decleretion functions */
void mvPresteraBh(void * arg);

#define ISR_DEBUG 1

/*******************************************************************************
* Internal usage variables
*******************************************************************************/
int short_bh_count = 0;

/*******************************************************************************
* prestera_tl_ISR
*
* DESCRIPTION:
*       This is the Prestera ISR reponsible for only signaling of the zccurence of
*       an event, any procecing will be done in the tasklet it self.
*
* INPUTS:
*       intrScanArrayLen- intrScanArray Length.
*       intrScanArray   - Array of hierarchy tree of interrupt scan struct to
*                         initialize.
*       scanArrayIdx    - start index in scan array to use.
*       subIntrScan     - Pointer to be assigned with scan tree.
*
* OUTPUTS:
*       scanArrayIdx    - New start index in scan array to use.
*       subIntrScan     - Pointer to scan tree created.
*
* RETURNS:
*       INT_OK on success, or
*       INT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void prestera_tl_ISR(int irq, void *dev_id, struct pt_regs *regs)
{
    struct tq_struct * tq;
    
    disable_irq(irq);    
    tq = (struct tq_struct *) dev_id;
    
    /* enqueue the PP task BH in the immediate queue */
    queue_task(tq, &tq_immediate);
    mark_bh(IMMEDIATE_BH);
    
    
    short_bh_count++;
}

/*******************************************************************************
* mvPresteraBh
*
* DESCRIPTION:
*       This is the Prestera ISR reponsible for only signaling of the zccurence of
*       an event, any procecing will be done in the tasklet it self.
*e);
* INPUTS:
*       intrScanArrayLen- intrScanArray Length.
*       intrScanArray   - Array of hierarchy tree of interrupt scan struct to
*                         initialize.
*       scanArrayIdx    - start index in scan array to use.
*       subIntrScan     - Pointer to be assigned with scan tree.
*
* OUTPUTS:
*       scanArrayIdx    - New start index in scan array to use.
*       subIntrScan     - Pointer to scan tree created.
*
* RETURNS:
*       INT_OK on success, or
*       INT_FAIL otherwise.
*e);
* COMMENTS:
*       None.
*
* Tasklet
* here we are processing the entire prestera event cause register,
* during the tree walk interrupt is disabled and all couse registers are cleared
**********************************************************************************************/
void mvPresteraBh (void * data)
{
    struct InterruptData * intData;

    intData = (struct InterruptData *) data;
    /* awake any reading process */
    up(&intData->sem);
    enable_irq(intData->vector);    
}

/*******************************************************************************
* intConnect
*
* DESCRIPTION:
*       connect and interrupt via register it at the kernel.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       The array size in entries.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
unsigned int intConnect(unsigned int vector, void * routine, struct InterruptData **cookie)
{
    unsigned int status;
    struct tq_struct * tq;
    struct InterruptData * intData;
    /* Disable all Prestera interrupst by clearing the mask reg */
    /* Interrupts will be re-enabled by the user */
    /* Parameter == device */
    /* writeLong(parameter,  0x00000118, 0); */
    /* crossbar writeLong(parameter,  0x40000084, 0); */
    /* crossbar writeLong(parameter,  0x0007FF7F, 0); */
    /* enable - writeLong(0x10,  0x00000118, 0x7ff); */
    
    tq = (struct tq_struct *) kmalloc(sizeof(struct tq_struct), GFP_KERNEL);
    if (NULL == tq)
    {
        printk("kmalloc failed\n");
        return -ENOMEM;
    }
    
    intData = (struct InterruptData *) kmalloc(sizeof(struct InterruptData), GFP_KERNEL);
    if (NULL == intData)
    {
        printk("kmalloc failed\n");
        return -ENOMEM;
    }
    *cookie = intData;
    
    /* The user process will wait on it */
    sema_init(&intData->sem, 0);
    intData->vector = vector;
    
    /* For cleanup we will need the tasklet */
    intData->tq = tq;
    
    INIT_TQUEUE(tq, mvPresteraBh, (void*)intData);

    status = request_irq(vector, prestera_tl_ISR, SA_INTERRUPT, "mvPP", (void *)tq);

    if (status)
    {
        panic("Can not assign IRQ to PresteraDev\n");
        return (INT_ERROR);
    }
    else
    {
        printk(KERN_DEBUG "PresteraOpen: connected Prestera IRQ - %d\n", vector);
        disable_irq(vector);
        return (INT_OK);
    }
}

