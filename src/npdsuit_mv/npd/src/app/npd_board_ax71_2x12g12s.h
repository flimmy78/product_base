#ifndef __NPD_BOARD_AX71_2X12G12S_H__
#define __NPD_BOARD_AX71_2X12G12S_H__

#define NAM_ASIC_AUXI_PHY_WRITE_CHECK(old_dataPtr,ret,devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, dataPtr) \
    if(RET_SUCCESS != ret)                                                                                             \
    {                                                                                                              \
        syslog_ax_board_err("nam_asic_auxi_phy_write fail, ret=%d\n", ret);                                        \
        return RET_FAIL;                                                                                           \
    }                                                                                                              \
    else                                                                                                               \
    {                                                                                                              \
        old_dataPtr = dataPtr;                                                                                     \
        nam_asic_auxi_phy_read(devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, &dataPtr);                  \
        syslog_ax_board_dbg(" write data is %x read data is %x,regAddr is %x\n",old_dataPtr,dataPtr,phyReg);	   \
        if(old_dataPtr != dataPtr)                                                                                 \
        return RET_FAIL;                                                                                       \
    }

extern int npd_init_ax71_2x12g12s(void);
extern void mac_config_for_phy_QT2225(void);

extern asic_board_t ax71_2x12g12s_board_t;

#endif
