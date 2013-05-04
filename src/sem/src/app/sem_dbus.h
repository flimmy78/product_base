/******************************************************************************
Copyright (C) Autelan Technology

This software file is owned and distributed by Autelan Technology 
*******************************************************************************

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
*******************************************************************************
* sem_dbus.h
*
*
* DESCRIPTION:
*  SEM Module Dbus implement.
*
* DATE:
*  2010-04-09
*
* CREATOR:
*  zhangdx@autelan.com
*
* CHANGE LOG:
*  2011-04-09 <zhangdx> Create file.
*
******************************************************************************/

#ifndef _SEM_DBUS_H
#define _SEM_DBUS_H

#define DBUS_TIMEOUT 500
#if 0
enum {
	SEM_COMMAND_SUCCESS,
	SEM_COMMAND_FAILED,
	SEM_WRONG_PARAM,
	SEM_COMMAND_NOT_SUPPORT
};

#endif

void sem_dbus_main();


unsigned int sem_dbus_power_state_abnormal
(
	unsigned int power_state_flg,
	unsigned int power_index
);
unsigned int sem_dbus_system_state_trap(unsigned int system_state);

#endif
