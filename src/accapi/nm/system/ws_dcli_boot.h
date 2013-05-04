/* cgicTempDir is the only setting you are likely to need
	to change in this file. */

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
* ws_dcli_boot.h
*
*
* CREATOR:
* autelan.software.Network Dep. team
* qiaojie@autelan.com
*
* DESCRIPTION:
*
*
*
*******************************************************************************/


#include "ws_init_dbus.h"
#include <dbus/dbus.h>

#include <ctype.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h> 
#include <sys/ioctl.h>
#include <ctype.h>
//#include <zebra.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
	
//#include "command.h"
//#include "dcli_boot.h"
//#include "image.h"
//#include <time.h>

extern int get_boot_img_name(char* imgname);
extern int config_boot_img_func_cmd(char *img_name);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾThe boot file should be .img file*/
														  /*����-2��ʾThe boot img doesn't exist������-3��ʾSysetm internal error (1)*/
														  /*����-4��ʾSysetm internal error (2)������-5��ʾStorage media is busy*/
														  /*����-6��ʾStorage operation time out������-7��ʾNo left space on storage media*/
														  /*����-8��ʾSysetm internal error (3)*/

