/*
 *
 * OCTEON SDK
 *
 * Copyright (c) 2011 Cavium Networks. All rights reserved.
 *
 * This file, which is part of the OCTEON SDK which also includes the
 * OCTEON SDK Package from Cavium Networks, contains proprietary and
 * confidential information of Cavium Networks and in some cases its
 * suppliers. 
 *
 * Any licensed reproduction, distribution, modification, or other use of
 * this file or the confidential information or patented inventions
 * embodied in this file is subject to your license agreement with Cavium
 * Networks. Unless you and Cavium Networks have agreed otherwise in
 * writing, the applicable license terms "OCTEON SDK License Type 5" can be found 
 * under the directory: $OCTEON_ROOT/components/driver/licenses/
 *
 * All other use and disclosure is prohibited.
 *
 * Contact Cavium Networks at info@caviumnetworks.com for more information.
 *
 */



/*! \file  cavium_sysdep.h
    \brief Host Driver: This file pulls in the OS-dependent header files.
*/

#ifndef _CAVIUM_SYSDEP_H
#define _CAVIUM_SYSDEP_H

#ifndef __CAVIUM_LITTLE_ENDIAN
#define __CAVIUM_LITTLE_ENDIAN 1234
#endif

#ifndef __CAVIUM_BIG_ENDIAN
#define __CAVIUM_BIG_ENDIAN	4321
#endif

#ifdef linux
#include "../driver/linux/linux_sysdep.h"
#elif defined(__FreeBSD__)
#include "../driver/freebsd/freebsd_sysdep.h"
#elif defined (_WIN32)
#include "..\driver\windows\windows_sysdep.h"
#else /* CUSTOM_OS */
#include "custom_sysdep.h"
#endif


#endif /* _CAVIUM_SYSDEP_H*/

/* $Id: cavium_sysdep.h 42868 2009-05-19 23:57:52Z panicker $ */
