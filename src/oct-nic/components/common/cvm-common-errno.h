/***********************************************************************

  OCTEON TOOLKITS                                                         
  Copyright (c) 2007 Cavium Networks. All rights reserved.

  This file, which is part of the OCTEON TOOLKIT from Cavium Networks,
  contains proprietary and confidential information of Cavium Networks
  and in some cases its suppliers.

  Any licensed reproduction, distribution, modification, or other use of
  this file or confidential information embodied in this file is subject
  to your license agreement with Cavium Networks. The applicable license
  terms can be found by contacting Cavium Networks or the appropriate
  representative within your company.

  All other use and disclosure is prohibited.

  Contact Cavium Networks at info@caviumnetworks.com for more information.

 ************************************************************************/ 

/*
 * Copyright (c) 1982, 1986, 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)errno.h	8.5 (Berkeley) 1/21/94
 * $FreeBSD: src/sys/sys/errno.h,v 1.26 2004/04/07 04:19:49 imp Exp $
 */


/** 
 * @file cvm-common-errno.h
 *
 * Description: Cavium specific error codes
 *
 * File version info: $Id: cvm-common-errno.h 27539 2007-08-24 22:07:42Z fmasood $ $Name$
 */

#ifndef __CVM_COMMON_ERRNO_H__
#define __CVM_COMMON_ERRNO_H__

#ifndef __KERNEL__

#ifndef _REENT_ONLY
#define errno (*__errno())
extern int *__errno (void);
#endif

extern int _sys_nerr;

#define __errno_r(ptr) ((ptr)->_errno)

/* --- end of slight redundancy (the use of struct _reent->_errno is
 *        hard-coded in perror.c so why pretend anything else could work too ?
 *        */

#define __set_errno(x) (errno = (x))

#endif /* __KERNEL__ */

#define BSD_COMPLIANCE_ERROR_CODES

#ifdef  BSD_COMPLIANCE_ERROR_CODES

#define	CVM_COMMON_EPERM		1		/* Operation not permitted */
#define	CVM_COMMON_ENOENT		2		/* No such file or directory */
#define	CVM_COMMON_ESRCH		3		/* No such process */
#define	CVM_COMMON_EINTR		4		/* Interrupted system call */
#define	CVM_COMMON_EIO			5		/* Input/output error */
#define	CVM_COMMON_ENXIO		6		/* Device not configured */
#define	CVM_COMMON_E2BIG		7		/* Argument list too long */
#define	CVM_COMMON_ENOEXEC		8		/* Exec format error */
#define	CVM_COMMON_EBADF		9		/* Bad file descriptor */
#define	CVM_COMMON_ECHILD		10		/* No child processes */
#define	CVM_COMMON_EDEADLK		11		/* Resource deadlock avoided */
					/* 11 was EAGAIN */
#define	CVM_COMMON_ENOMEM		12		/* Cannot allocate memory */
#define	CVM_COMMON_EACCES		13		/* Permission denied */
#define	CVM_COMMON_EFAULT		14		/* Bad address */
#ifndef _POSIX_SOURCE
#define	CVM_COMMON_ENOTBLK		15		/* Block device required */
#endif
#define	CVM_COMMON_EBUSY		16		/* Device busy */
#define	CVM_COMMON_EEXIST		17		/* File exists */
#define	CVM_COMMON_EXDEV		18		/* Cross-device link */
#define	CVM_COMMON_ENODEV		19		/* Operation not supported by device */
#define	CVM_COMMON_ENOTDIR		20		/* Not a directory */
#define	CVM_COMMON_EISDIR		21		/* Is a directory */
#define	CVM_COMMON_EINVAL		22		/* Invalid argument */
#define	CVM_COMMON_ENFILE		23		/* Too many open files in system */
#define	CVM_COMMON_EMFILE		24		/* Too many open files */
#define	CVM_COMMON_ENOTTY		25		/* Inappropriate ioctl for device */
#ifndef _POSIX_SOURCE
#define	CVM_COMMON_ETXTBSY		26		/* Text file busy */
#endif
#define	CVM_COMMON_EFBIG		27		/* File too large */
#define	CVM_COMMON_ENOSPC		28		/* No space left on device */
#define	CVM_COMMON_ESPIPE		29		/* Illegal seek */
#define	CVM_COMMON_EROFS		30		/* Read-only filesystem */
#define	CVM_COMMON_EMLINK		31		/* Too many links */
#define	CVM_COMMON_EPIPE		32		/* Broken pipe */

/* math software */
#define	CVM_COMMON_EDOM		33		/* Numerical argument out of domain */
#define	CVM_COMMON_ERANGE		34		/* Result too large */

/* non-blocking and interrupt i/o */
#define	CVM_COMMON_EAGAIN		35		/* Resource temporarily unavailable */
#ifndef _POSIX_SOURCE
#define	CVM_COMMON_EWOULDBLOCK	CVM_COMMON_EAGAIN	/* Operation would block */
#define	CVM_COMMON_EINPROGRESS	36		/* Operation now in progress */
#define	CVM_COMMON_EALREADY	37		/* Operation already in progress */

/* ipc/network software -- argument errors */
#define	CVM_COMMON_ENOTSOCK	38		/* Socket operation on non-socket */
#define	CVM_COMMON_EDESTADDRREQ	39		/* Destination address required */
#define	CVM_COMMON_EMSGSIZE	40		/* Message too long */
#define	CVM_COMMON_EPROTOTYPE	41		/* Protocol wrong type for socket */
#define	CVM_COMMON_ENOPROTOOPT	42		/* Protocol not available */
#define	CVM_COMMON_EPROTONOSUPPORT	43		/* Protocol not supported */
#define	CVM_COMMON_ESOCKTNOSUPPORT	44		/* Socket type not supported */
#define	CVM_COMMON_EOPNOTSUPP	45		/* Operation not supported */
#define	CVM_COMMON_ENOTSUP	CVM_COMMON_EOPNOTSUPP	/* Operation not supported */
#define	CVM_COMMON_EPFNOSUPPORT	46		/* Protocol family not supported */
#define	CVM_COMMON_EAFNOSUPPORT	47		/* Address family not supported by protocol family */
#define	CVM_COMMON_EADDRINUSE	48		/* Address already in use */
#define	CVM_COMMON_EADDRNOTAVAIL	49		/* Can't assign requested address */

/* ipc/network software -- operational errors */
#define	CVM_COMMON_ENETDOWN	50		/* Network is down */
#define	CVM_COMMON_ENETUNREACH	51		/* Network is unreachable */
#define	CVM_COMMON_ENETRESET	52		/* Network dropped connection on reset */
#define	CVM_COMMON_ECONNABORTED	53		/* Software caused connection abort */
#define	CVM_COMMON_ECONNRESET	54		/* Connection reset by peer */
#define	CVM_COMMON_ENOBUFS		55		/* No buffer space available */
#define	CVM_COMMON_EISCONN		56		/* Socket is already connected */
#define	CVM_COMMON_ENOTCONN	57		/* Socket is not connected */
#define	CVM_COMMON_ESHUTDOWN	58		/* Can't send after socket shutdown */
#define	CVM_COMMON_ETOOMANYREFS	59		/* Too many references: can't splice */
#define	CVM_COMMON_ETIMEDOUT	60		/* Operation timed out */
#define	CVM_COMMON_ECONNREFUSED	61		/* Connection refused */

#define	CVM_COMMON_ELOOP		62		/* Too many levels of symbolic links */
#endif /* _POSIX_SOURCE */
#define	CVM_COMMON_ENAMETOOLONG	63		/* File name too long */

/* should be rearranged */
#ifndef _POSIX_SOURCE
#define	CVM_COMMON_EHOSTDOWN	64		/* Host is down */
#define	CVM_COMMON_EHOSTUNREACH	65		/* No route to host */
#endif /* _POSIX_SOURCE */
#define	CVM_COMMON_ENOTEMPTY	66		/* Directory not empty */

/* quotas & mush */
#ifndef _POSIX_SOURCE
#define	CVM_COMMON_EPROCLIM	67		/* Too many processes */
#define	CVM_COMMON_EUSERS		68		/* Too many users */
#define	CVM_COMMON_EDQUOT		69		/* Disc quota exceeded */

/* Network File System */
#define	CVM_COMMON_ESTALE		70		/* Stale NFS file handle */
#define	CVM_COMMON_EREMOTE		71		/* Too many levels of remote in path */
#define	CVM_COMMON_EBADRPC		72		/* RPC struct is bad */
#define	CVM_COMMON_ERPCMISMATCH	73		/* RPC version wrong */
#define	CVM_COMMON_EPROGUNAVAIL	74		/* RPC prog. not avail */
#define	CVM_COMMON_EPROGMISMATCH	75		/* Program version wrong */
#define	CVM_COMMON_EPROCUNAVAIL	76		/* Bad procedure for program */
#endif /* _POSIX_SOURCE */

#define	CVM_COMMON_ENOLCK		77		/* No locks available */
#define	CVM_COMMON_ENOSYS		78		/* Function not implemented */

#ifndef _POSIX_SOURCE
#define	CVM_COMMON_EFTYPE		79		/* Inappropriate file type or format */
#define	CVM_COMMON_EAUTH		80		/* Authentication error */
#define	CVM_COMMON_ENEEDAUTH	81		/* Need authenticator */
#define	CVM_COMMON_EIDRM		82		/* Identifier removed */
#define	CVM_COMMON_ENOMSG		83		/* No message of desired type */
#define	CVM_COMMON_EOVERFLOW	84		/* Value too large to be stored in data type */
#define	CVM_COMMON_ECANCELED	85		/* Operation canceled */
#define	CVM_COMMON_EILSEQ		86		/* Illegal byte sequence */
#define	CVM_COMMON_ENOATTR		87		/* Attribute not found */

#define CVM_COMMON_EDOOFUS		88		/* Programming error */

#define	CVM_COMMON_ELAST		88		/* Must be equal largest errno */

#endif /* _POSIX_SOURCE */

#ifdef _KERNEL
/* pseudo-errors returned inside kernel to modify return to process */
#define	CVM_COMMON_ERESTART	(-1)		/* restart syscall */
#define	CVM_COMMON_EJUSTRETURN	(-2)		/* don't modify regs, just return */
#define	CVM_COMMON_ENOIOCTL	(-3)		/* ioctl not handled by this layer */
#define	CVM_COMMON_EDIRIOCTL	(-4)		/* do direct ioctl in GEOM */
#endif

#else /* BSD_COMPLIANCE_ERROR_CODES */

#define CVM_COMMON_EPERM 1	/* Not super-user */
#define CVM_COMMON_ENOENT 2	/* No such file or directory */
#define CVM_COMMON_ESRCH 3	/* No such process */
#define CVM_COMMON_EINTR 4	/* Interrupted system call */
#define CVM_COMMON_EIO 5	/* I/O error */
#define CVM_COMMON_ENXIO 6	/* No such device or address */
#define CVM_COMMON_E2BIG 7	/* Arg list too long */
#define CVM_COMMON_ENOEXEC 8	/* Exec format error */
#define CVM_COMMON_EBADF 9	/* Bad file number */
#define CVM_COMMON_ECHILD 10	/* No children */
#define CVM_COMMON_EAGAIN 11	/* No more processes */
#define CVM_COMMON_ENOMEM 12	/* Not enough core */
#define CVM_COMMON_EACCES 13	/* Permission denied */
#define CVM_COMMON_EFAULT 14	/* Bad address */
#define CVM_COMMON_ENOTBLK 15	/* Block device required */
#define CVM_COMMON_EBUSY 16	/* Mount device busy */
#define CVM_COMMON_EEXIST 17	/* File exists */
#define CVM_COMMON_EXDEV 18	/* Cross-device link */
#define CVM_COMMON_ENODEV 19	/* No such device */
#define CVM_COMMON_ENOTDIR 20	/* Not a directory */
#define CVM_COMMON_EISDIR 21	/* Is a directory */
#define CVM_COMMON_EINVAL 22	/* Invalid argument */
#define CVM_COMMON_ENFILE 23	/* Too many open files in system */
#define CVM_COMMON_EMFILE 24	/* Too many open files */
#define CVM_COMMON_ENOTTY 25	/* Not a typewriter */
#define CVM_COMMON_ETXTBSY 26	/* Text file busy */
#define CVM_COMMON_EFBIG 27	/* File too large */
#define CVM_COMMON_ENOSPC 28	/* No space left on device */
#define CVM_COMMON_ESPIPE 29	/* Illegal seek */
#define CVM_COMMON_EROFS 30	/* Read only file system */
#define CVM_COMMON_EMLINK 31	/* Too many links */
#define CVM_COMMON_EPIPE 32	/* Broken pipe */
#define CVM_COMMON_EDOM 33	/* Math arg out of domain of func */
#define CVM_COMMON_ERANGE 34	/* Math result not representable */
#define CVM_COMMON_ENOMSG 35	/* No message of desired type */
#define CVM_COMMON_EIDRM 36	/* Identifier removed */
#define CVM_COMMON_ECHRNG 37	/* Channel number out of range */
#define CVM_COMMON_EL2NSYNC 38	/* Level 2 not synchronized */
#define CVM_COMMON_EL3HLT 39	/* Level 3 halted */
#define CVM_COMMON_EL3RST 40	/* Level 3 reset */
#define CVM_COMMON_ELNRNG 41	/* Link number out of range */
#define CVM_COMMON_EUNATCH 42	/* Protocol driver not attached */
#define CVM_COMMON_ENOCSI 43	/* No CSI structure available */
#define CVM_COMMON_EL2HLT 44	/* Level 2 halted */
#define CVM_COMMON_EDEADLK 45	/* Deadlock condition */
#define CVM_COMMON_ENOLCK 46	/* No record locks available */
#define CVM_COMMON_EBADE 50	/* Invalid exchange */
#define CVM_COMMON_EBADR 51	/* Invalid request descriptor */
#define CVM_COMMON_EXFULL 52	/* Exchange full */
#define CVM_COMMON_ENOANO 53	/* No anode */
#define CVM_COMMON_EBADRQC 54	/* Invalid request code */
#define CVM_COMMON_EBADSLT 55	/* Invalid slot */
#define CVM_COMMON_EDEADLOCK 56	/* File locking deadlock error */
#define CVM_COMMON_EBFONT 57	/* Bad font file fmt */
#define CVM_COMMON_ENOSTR 60	/* Device not a stream */
#define CVM_COMMON_ENODATA 61	/* No data (for no delay io) */
#define CVM_COMMON_ETIME 62	/* Timer expired */
#define CVM_COMMON_ENOSR 63	/* Out of streams resources */
#define CVM_COMMON_ENONET 64	/* Machine is not on the network */
#define CVM_COMMON_ENOPKG 65	/* Package not installed */
#define CVM_COMMON_EREMOTE 66	/* The object is remote */
#define CVM_COMMON_ENOLINK 67	/* The link has been severed */
#define CVM_COMMON_EADV 68	/* Advertise error */
#define CVM_COMMON_ESRMNT 69	/* Srmount error */
#define CVM_COMMON_ECOMM 70	/* Communication error on send */
#define CVM_COMMON_EPROTO 71	/* Protocol error */
#define CVM_COMMON_EMULTIHOP 74	/* Multihop attempted */
#define CVM_COMMON_ELBIN 75	/* Inode is remote (not really error) */
#define CVM_COMMON_EDOTDOT 76	/* Cross mount point (not really error) */
#define CVM_COMMON_EBADMSG 77	/* Trying to read unreadable message */
#define CVM_COMMON_EFTYPE 79	/* Inappropriate file type or format */
#define CVM_COMMON_ENOTUNIQ 80	/* Given log. name not unique */
#define CVM_COMMON_EBADFD 81	/* f.d. invalid for this operation */
#define CVM_COMMON_EREMCHG 82	/* Remote address changed */
#define CVM_COMMON_ELIBACC 83	/* Can't access a needed shared lib */
#define CVM_COMMON_ELIBBAD 84	/* Accessing a corrupted shared lib */
#define CVM_COMMON_ELIBSCN 85	/* .lib section in a.out corrupted */
#define CVM_COMMON_ELIBMAX 86	/* Attempting to link in too many libs */
#define CVM_COMMON_ELIBEXEC 87	/* Attempting to exec a shared library */
#define CVM_COMMON_ENOSYS 88	/* Function not implemented */
#define CVM_COMMON_ENMFILE 89      /* No more files */
#define CVM_COMMON_ENOTEMPTY 90	/* Directory not empty */
#define CVM_COMMON_ENAMETOOLONG 91	/* File or path name too long */
#define CVM_COMMON_ELOOP 92	/* Too many symbolic links */
#define CVM_COMMON_EOPNOTSUPP 95	/* Operation not supported on transport endpoint */
#define CVM_COMMON_EPFNOSUPPORT 96 /* Protocol family not supported */
#define CVM_COMMON_ECONNRESET 104  /* Connection reset by peer */
#define CVM_COMMON_ENOBUFS 105	/* No buffer space available */
#define CVM_COMMON_EAFNOSUPPORT 106 /* Address family not supported by protocol family */
#define CVM_COMMON_EPROTOTYPE 107	/* Protocol wrong type for socket */
#define CVM_COMMON_ENOTSOCK 108	/* Socket operation on non-socket */
#define CVM_COMMON_ENOPROTOOPT 109	/* Protocol not available */
#define CVM_COMMON_ESHUTDOWN 110	/* Can't send after socket shutdown */
#define CVM_COMMON_ECONNREFUSED 111	/* Connection refused */
#define CVM_COMMON_EADDRINUSE 112		/* Address already in use */
#define CVM_COMMON_ECONNABORTED 113	/* Connection aborted */
#define CVM_COMMON_ENETUNREACH 114		/* Network is unreachable */
#define CVM_COMMON_ENETDOWN 115		/* Network interface is not configured */
#define CVM_COMMON_ETIMEDOUT 116		/* Connection timed out */
#define CVM_COMMON_EHOSTDOWN 117		/* Host is down */
#define CVM_COMMON_EHOSTUNREACH 118	/* Host is unreachable */
#define CVM_COMMON_EINPROGRESS 119		/* Connection already in progress */
#define CVM_COMMON_EALREADY 120		/* Socket already connected */
#define CVM_COMMON_EDESTADDRREQ 121	/* Destination address required */
#define CVM_COMMON_EMSGSIZE 122		/* Message too long */
#define CVM_COMMON_EPROTONOSUPPORT 123	/* Unknown protocol */
#define CVM_COMMON_ESOCKTNOSUPPORT 124	/* Socket type not supported */
#define CVM_COMMON_EADDRNOTAVAIL 125	/* Address not available */
#define CVM_COMMON_ENETRESET 126
#define CVM_COMMON_EISCONN 127		/* Socket is already connected */
#define CVM_COMMON_ENOTCONN 128		/* Socket is not connected */
#define CVM_COMMON_ETOOMANYREFS 129
#define CVM_COMMON_EPROCLIM 130
#define CVM_COMMON_EUSERS 131
#define CVM_COMMON_EDQUOT 132
#define CVM_COMMON_ESTALE 133
#define CVM_COMMON_ENOTSUP 134		/* Not supported */
#define CVM_COMMON_ENOMEDIUM 135   /* No medium (in tape drive) */
#define CVM_COMMON_ENOSHARE 136    /* No such host or network path */
#define CVM_COMMON_ECASECLASH 137  /* Filename exists with different case */
#define CVM_COMMON_EILSEQ 138
#define CVM_COMMON_EOVERFLOW 139	/* Value too large for defined data type */

#endif /* BSD_COMPLIANCE_ERROR_CODES */

#endif /* __CVM_COMMON_ERRNO_H__ */
