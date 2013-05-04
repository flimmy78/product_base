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
********************************************************************************
* prestera_glob.h
*
* DESCRIPTION:
*       This file includes the declaration of the struct we want to send to kernel mode,
*       from user mode.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __INCPresteraGlobh
#define __INCPresteraGlobh

typedef struct pair
{
    unsigned int 	slvId;
    unsigned int 	regAddr;
    unsigned int 	value;
} PP_PCI_REG;

typedef struct write_field_STCT
{
    unsigned int 	slvId;
    unsigned int 	regAddr;
    unsigned int 	mask;
    unsigned int 	value;
} write_pci_reg_field_STC;

typedef struct PciConfigWriteReg_STCT
{
    unsigned int   busNo;
    unsigned int   devSel;
    unsigned int   funcNo;
    unsigned int   regAddr;
    unsigned int   data;
} PciConfigReg_STC;

typedef struct
{
    unsigned short 	vendorId;
    unsigned short 	devId;
    unsigned int  	instance;
    unsigned int  	busNo;
    unsigned int  	devSel;
    unsigned int  	funcNo;
} GT_PCI_Dev_STC;

typedef struct
{
    unsigned int 	intrLine;
    unsigned int 	busNo;      /* bus number */
    unsigned int 	deviceNo;   /* device number  */
	unsigned int 	funcNo;	 /* function number */
    unsigned int 	vector;     /* INT vector number */
} GT_Intr2Vec;

typedef struct
{
    unsigned int 	vector;
    unsigned int 	cookieLow32;
	unsigned int 	cookieHigh32;
} GT_VecotrCookie_STC;

typedef struct
{
    unsigned int 	address;
    unsigned int 	length;
} GT_RANGE_STC;

#ifndef __AX_PLATFORM__
typedef struct
{
    unsigned int  	busNo;		/* bus number */
    unsigned int  	devSel;		/* device number */
    unsigned int  	funcNo;		/* function number */
	unsigned int 	dma_base; 	/* dma base used by CPU */
	unsigned int  	device_base;	/* dma base used by device */
}GT_Dma_Base_STC;

typedef struct 
{
	unsigned char 	isRead;
	unsigned int 	address;
	unsigned int 	data;
}GT_RAWIOWR_STC;

typedef struct
{
	unsigned int 	low32;
	unsigned int 	high32;
}GT_Cookie_Ptr_STC;

struct tx_s {
	unsigned int excessiveCollision;
	unsigned int goodbytesl;
	unsigned int goodbytesh;
	unsigned int uncastframe;
	unsigned int bcastframe;
	unsigned int mcastframe;
	unsigned int fcframe;
	unsigned int sentMutiple;
	unsigned int crcerror_fifooverrun;
	unsigned int send_deferred;
	unsigned int frame;/*all of unicast multicast & bcast*/
	unsigned int dropped;/**/
	unsigned int errors;/**/	
};
struct rx_s {
	unsigned int uncastframes;
	unsigned int bcastframes;
	unsigned int mcastframes;
	unsigned int fifooverruns;
	unsigned int goodbytesl;
	unsigned int goodbytesh;
	unsigned int badbytes;
	unsigned int fcframe;
	unsigned int errorframe;
	unsigned int jabber;
	unsigned int underSizepkt;
	unsigned int overSizepkt;
	unsigned int fragments;
	unsigned int dropped;/**/
	unsigned int carrier;/**/
};

struct port_oct_s{
		unsigned int late_collision; 
		unsigned int collision;
		unsigned int badCRC;
		unsigned int b1024oct2max;
		unsigned int b512oct21023;
		unsigned int b256oct511;
		unsigned int b128oct255;
		unsigned int b64oct127;
		unsigned int b64oct;
};

#if 0
struct tx_s {
	unsigned int 	packets; /*just be the same with frams.*/
	unsigned int 	errors;	/*CRC errorPkts_RX*/
	unsigned int 	dropped;	/**/
	unsigned int 	overruns;/*Fifo OverRun*/
	unsigned int 	frame;
	unsigned int 	goodbytesl;
	unsigned int 	goodbytesh;
	unsigned int	uncastframe;
	unsigned int	bcastframe;
	unsigned int 	fcframe;
	unsigned int	mcastframe;
	unsigned int	crcerror_fifooverrun;
};
struct rx_s {
	unsigned int uncastpkts;
	unsigned int bcastpkts;
	unsigned int mcastpkts;
	unsigned int CRCerrors;
	unsigned int dropped;
	unsigned int overruns;
	unsigned int carrier;
	unsigned int collision;
	unsigned int goodbytesl;
	unsigned int goodbytesh;
	unsigned int badbytes;
	unsigned int fcframe;
	unsigned int errorframe;
	unsigned int jabber;
	unsigned int underSizeframe;
	unsigned int overSizeframe;
	unsigned int fragments;
};
#endif

typedef struct
{
	unsigned char devNum;
	unsigned char portNum;
	unsigned int mode;
	struct rx_s rx;
	struct tx_s tx;
}GT_PKG_COUNT_STAT;

typedef struct 
{
	unsigned char devNum;
	unsigned char portNum;

}GT_DEV_PORT;

typedef struct
{
	unsigned int	regAddr; 	/* CPU register address */
	unsigned int 	value;		/* CPu register value */
	int				opResult;	/* operation result: 0 - success, other - failed */
} GT_CpuRegReadWrite_STC;

#endif

#define PRESTERA_IOC_MAGIC 'p'
#define PRESTERA_IOC_HWRESET           _IO(PRESTERA_IOC_MAGIC,   0)
#define PRESTERA_IOC_WRITEREG          _IOW(PRESTERA_IOC_MAGIC,  1, PP_PCI_REG)
#define PRESTERA_IOC_READREG           _IOWR(PRESTERA_IOC_MAGIC, 2, PP_PCI_REG)
#define PRESTERA_IOC_INTCONNECT        _IOWR(PRESTERA_IOC_MAGIC, 3, GT_VecotrCookie_STC)
#define PRESTERA_IOC_INTENABLE         _IOW(PRESTERA_IOC_MAGIC,  4, int)
#define PRESTERA_IOC_INTDISABLE        _IOW(PRESTERA_IOC_MAGIC,  5, int)
#define PRESTERA_IOC_WAIT              _IOW(PRESTERA_IOC_MAGIC,  6, GT_Cookie_Ptr_STC)
#define PRESTERA_IOC_FIND_DEV          _IOWR(PRESTERA_IOC_MAGIC, 7, GT_PCI_Dev_STC) 
#define PRESTERA_IOC_PCICONFIGWRITEREG _IOW(PRESTERA_IOC_MAGIC,  8, PciConfigReg_STC)
#define PRESTERA_IOC_PCICONFIGREADREG  _IOWR(PRESTERA_IOC_MAGIC, 9, PciConfigReg_STC)
#define PRESTERA_IOC_GETINTVEC         _IOWR(PRESTERA_IOC_MAGIC,10, GT_Intr2Vec)
#define PRESTERA_IOC_FLUSH             _IOW(PRESTERA_IOC_MAGIC, 11, GT_RANGE_STC)
#define PRESTERA_IOC_INVALIDATE        _IOW(PRESTERA_IOC_MAGIC, 12, GT_RANGE_STC)
#define PRESTERA_IOC_GETBASEADDR       _IOR(PRESTERA_IOC_MAGIC, 13, int*)
#define PRESTERA_IOC_GETPKGSTACOUNT       _IOR(PRESTERA_IOC_MAGIC, 14, GT_PKG_COUNT_STAT)
#define PRESTERA_IOC_CLAERPKGCOUNT       _IOR(PRESTERA_IOC_MAGIC, 15,GT_DEV_PORT)
#define MAGIC 054217146
#define MAX_LEN 64
#ifndef __AX_PLATFORM__
#define PRESTERA_IOC_RAWMEMRW         _IOWR(PRESTERA_IOC_MAGIC,	14, GT_RAWIOWR_STC)
#define PRESTERA_IOC_GETDEVBASEADDR   _IOR(PRESTERA_IOC_MAGIC, 15, GT_Dma_Base_STC)
#define PRESTERA_IOC_CPUREGREAD		   _IOR(PRESTERA_IOC_MAGIC, 25, GT_CpuRegReadWrite_STC)
#define PRESTERA_IOC_CPUREGWRITE	   _IOW(PRESTERA_IOC_MAGIC, 26, GT_CpuRegReadWrite_STC)
#endif
#endif /* __INCPresteraGlobh */
