#ifndef __NAM_DYNAMIC_TRUNK_H__
#define __NAM_DYNAMIC_TRUNK_H__
#define __CPSSDxCh__


#include "nam_log.h"
#define MAX_DYNAMIC_TRUNK_MEMBER (0x8)  /*max port count of one dynamic trunk*/

int nam_dynamic_trunk_lacp_trap_init
(
	unsigned char devNum
);
int nam_asic_show_dynamic_trunk_hwinfo
(		
		unsigned short trunkId,
		unsigned long *numOfEnabledMembersPtr,
		unsigned long *numOfDisabledMembersPtr,
		CPSS_TRUNK_MEMBER_STC  *enabledMembersArray,
		CPSS_TRUNK_MEMBER_STC  *disabledMembersArray
);
extern unsigned long nam_asic_get_instance_num
(
	void
);

#ifdef __CPSSDxCh__
extern unsigned long cpssDxChTrafficFullQueueTrapInit
(
	unsigned char devNum,
	unsigned int portNum
);

extern unsigned long cpssDxChBrgGenIeeeReservedMcastProtCmdSet
(
     unsigned char                dev,
#ifdef DRV_LIB_CPSS_3_4
     unsigned int               profileIndex,
#endif
     unsigned char                protocol,
     unsigned int  				  cmd
);

extern unsigned long cpssDxChNetIfCpuCodeIeeeReservedMcastProtSet
(
     unsigned char            devNum,
     unsigned char            protocol,
     unsigned int			  cpuCode
);

extern unsigned long cpssDxChBrgGenIeeeReservedMcastTrapEnable
(
     unsigned char    dev,
     unsigned long    enable
);
#endif

#endif

