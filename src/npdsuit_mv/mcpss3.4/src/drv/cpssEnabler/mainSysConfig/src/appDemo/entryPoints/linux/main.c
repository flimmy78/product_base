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
* main.c
*
* DESCRIPTION:
*       linux entry point implementation
*
*
* DEPENDENCIES:
*
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
#include <gtOs/gtOsSharedMemory.h>
#include <gtOs/gtOsInit.h>

extern void npd_init_tell_whoami
(
	unsigned char *tName,
	unsigned char lastTeller
);


#ifndef __AX_PLATFORM__
extern unsigned int auteware_product_id;
#endif

extern int userAppInit(void);

void preUserAppInit(void)
{
    /* here we will map the pss memory */
    
    userAppInit();
}

#ifdef __CPSS_DRV_SHLIB__
int cpssUserAppMain
(
    unsigned int product_id
)

{
    auteware_product_id = product_id;

	/* tell my thead id */
	npd_init_tell_whoami("AsicInit",0);

    #ifdef SHARED_MEMORY
    if(shrMemInitSharedLibrary_FirstClient() != GT_OK)
        return -1;
    #endif

    if (osStartEngine(0, NULL, "appDemo", preUserAppInit) != GT_OK)
        return 1;

    return 0;
}
#endif


int main(int argc, const char * argv[])
{

#ifdef SHARED_MEMORY
    if(shrMemInitSharedLibrary_FirstClient() != GT_OK)
        return -1;
#endif

    if (osStartEngine(argc, argv, "appDemo", preUserAppInit) != GT_OK)
        return 1;

    return 0;
}

