#ifndef _CAVIUM_RATELIMIT_H
#define _CAVIUM_RATELIMIT_H

#define CVM_RCV_PROTOCOL_UNKNOWN 0

#define MAX_MATCH_RULES_NUM 32
#define CVM_RATE_LIMIT_DEFAULT_RULE_NUM 24
#define PACKET_MATCH_BYTE_NUM 16
#define PROTOCOL_NAME_LEN 32
#define CVM_RATE_LIMIT_SECOND (1000)

#define CVM_RATE_NO_LIMIT 5000001

#define CR_IOC_MAGIC 244
#define CR_MAXNR 255
#define ETH_TYPE_8021Q 0x8100

/*MATCH_TYPE only allowed to be uint or ulonglong*/
#define MATCH_TYPE unsigned long long
#define MATCH_TYPE_LEN sizeof(MATCH_TYPE)
#define STATISTIC_TYPE unsigned long long

#define CR_SET_FLAG_CVM_ETH  0x01
#define CR_SET_FLAG_FAST_FWD 0x02

#define RATELIMIT_TBL_NAME "ratelimit_tbl"


#define DYNAMIC_MAJOR_NUM

#ifndef DYNAMIC_MAJOR_NUM

#define CRADDRULES 		_IOWR(CR_IOC_MAGIC, 0x1, struct cr_ioctl_struct)
#define CRDELRULES 		_IOWR(CR_IOC_MAGIC, 0x2, struct cr_ioctl_struct)
#define CRMODIFYRULES 	_IOWR(CR_IOC_MAGIC, 0x3, struct cr_ioctl_struct)
#define CRMODIFYMASKS 	_IOWR(CR_IOC_MAGIC, 0x4, struct cr_ioctl_struct)
#define CRMODIFYLIMITER _IOWR(CR_IOC_MAGIC, 0x5, struct cr_ioctl_struct)
#define CRMODIFYNAME 	_IOWR(CR_IOC_MAGIC, 0x6, struct cr_ioctl_struct)
#define CRGETRULE    	_IOWR(CR_IOC_MAGIC, 0x7, struct cr_ioctl_struct)
#define CRGETRULEBYINDEX    	_IOWR(CR_IOC_MAGIC, 0x8, struct cr_ioctl_struct)
#define CRENABLESET		_IOWR(CR_IOC_MAGIC, 0x9, struct cr_ioctl_struct)
#define CRCHECKDEFAULT 		_IOWR(CR_IOC_MAGIC, 0xa, struct cr_ioctl_struct)
#define CRENABLEGET 		_IOWR(CR_IOC_MAGIC, 0xb, struct cr_ioctl_struct)
#define CRSTACOUNTGET		_IOWR(CR_IOC_MAGIC, 0xc, struct cr_ioctl_struct)
#define CRSTACOUNTCLEAR		_IOWR(CR_IOC_MAGIC, 0xd, struct cr_ioctl_struct)
#define CRDMESGENABLESET	_IOWR(CR_IOC_MAGIC, 0xe, struct cr_ioctl_struct)
#define CRCLEARUDFRULES	_IOWR(CR_IOC_MAGIC, 0xf, struct cr_ioctl_struct)
#define CRGETRULEBYIDX4FFWD	_IOWR(CR_IOC_MAGIC, 0x10, struct cr_ioctl_struct)
#define CRGETRULE4FFWD	_IOWR(CR_IOC_MAGIC, 0x11, struct cr_ioctl_struct)



#define IOCTLCMD(cmd)	cmd
#else
#define CRADDRULES 		0x1
#define CRDELRULES 		0x2
#define CRMODIFYRULES 	0x3
#define CRMODIFYMASKS 	0x4
#define CRMODIFYLIMITER 0x5
#define CRMODIFYNAME 	0x6
#define CRGETRULE    	0x7
#define CRGETRULEBYINDEX    	0x8
#define CRENABLESET		0x9
#define CRCHECKDEFAULT 		0xa
#define CRENABLEGET 		0xb
#define CRSTACOUNTGET		0xc
#define CRSTACOUNTCLEAR		0xd
#define CRDMESGENABLESET	0xe
#define CRCLEARUDFRULES		0xf
#define CRGETRULEBYIDX4FFWD 0x10
#define CRGETRULE4FFWD		0x11
#define IOCTLCMD(nr) _IOWR(cr_major_num, nr, struct cr_ioctl_struct)
#endif

extern int rate_limit_main(MATCH_TYPE * skbBuffPtr, int len);
extern int (* rcv_rate_limit_hook)(MATCH_TYPE *,int);


#endif
