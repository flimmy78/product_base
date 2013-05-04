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
* npd_diag.c
*
*
* CREATOR:
*		qinhs@autelan.com
*
* DESCRIPTION:
*		NPD implement for ASIC diagnosis configuration.
*
* DATE:
*		03/24/2009	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.20 $	
*******************************************************************************/
#ifdef __cplusplus
	extern "C"
	{
#endif
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "util/npd_list.h"
#include "sysdef/npd_sysdef.h"
#include "dbus/npd/npd_dbus_def.h"
#include "npd/nbm/npd_bmapi.h"
#include "npd/nam/npd_amapi.h"
#include "npd_log.h"
#include "nbm/nbm_api.h"
#include "npd_c_slot.h"
#include "npd_product.h"
#include "npd_main.h"
#include "npd_diag.h"
#include "sysdef/returncode.h"

/* to define the master info zhangdi@autelan.com 2012-03-23 */
extern BoardInfo board_info;

/**********************************************************************************
 * npd_dbus_diagnosis_phy_hw_rw_reg
 *		read/write phy register
 *
 * 	INPUT:
 *		DBusConnection *conn, 
 *		DBusMessage *msg, 
 *		void *user_data
 *
 *	OUTPUT:
 * 		register value
 *
 *	RETURN:
 * 	 	reply
 **********************************************************************************/
DBusMessage * npd_dbus_diagnosis_phy_hw_rw_reg(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	DBusMessage* reply;
	unsigned int ret = DIAG_RETURN_CODE_SUCCESS;

	unsigned int hwType = 0;
	unsigned int opType = 0;
	unsigned int opDevice = 0;
	unsigned short opPort = 0;
	unsigned short opRegaddr = 0;
	unsigned short regValue = 0xFFFF;
	unsigned int slotNo = 0, localPortNo = 0;
	
	DBusError err;
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,
									DBUS_TYPE_UINT32, &hwType,
									DBUS_TYPE_UINT32, &opType,
									DBUS_TYPE_UINT32, &opDevice,
									DBUS_TYPE_UINT16,  &opPort,
									DBUS_TYPE_UINT16,  &opRegaddr,
									DBUS_TYPE_UINT16, &regValue,
									DBUS_TYPE_INVALID))) {
		syslog_ax_product_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
				syslog_ax_product_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
		}
		ret = DIAG_RETURN_CODE_ERROR;
	}

	/* check if asic port given is valid */
	ret = nam_get_slotport_by_devport(opDevice, opPort,PRODUCT_ID, &slotNo, &localPortNo);
	if(!ret) {
		syslog_ax_product_dbg("diag port(%d,%d) on product %#x is %d/%d\n", 
				opDevice, opPort, PRODUCT_ID, slotNo, localPortNo);
		/* read phy reg*/
		switch (hwType) {
			case 1:
				if (opType == 0) {
					ret = nam_asic_phy_read(opDevice, opPort, opRegaddr, &regValue);
				}
				else if (opType == 1) {
					ret = nam_asic_phy_write(opDevice, opPort, opRegaddr, regValue);
				}
				else{
					 syslog_ax_product_err("ERROR: unit %d MII Addr 0x%#x: operate is not read/write\n",
						  opDevice, opPort);
					ret = DIAG_RETURN_CODE_ERROR;
				}
				break;
			default:
				syslog_ax_product_err("ERROR: hardware is error\n",
						  hwType);
				ret = DIAG_RETURN_CODE_ERROR;
		}
	}
	else {
		syslog_ax_product_err("diag port(%d,%d) is invalid on product %#x\n",
				opDevice, opPort, PRODUCT_ID);
		ret = DIAG_RETURN_CODE_ERROR;
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32, &ret,
 							 DBUS_TYPE_UINT16, &regValue,
							 DBUS_TYPE_INVALID);
	return reply;
}	

/**********************************************************************************
 * npd_dbus_diagnosis_pci_hw_rw_reg
 *		read/write pci register
 *
 * 	INPUT:
 *		DBusConnection *conn, 
 *		DBusMessage *msg, 
 *		void *user_data
 *
 *	OUTPUT:
 * 		register value
 *
 *	RETURN:
 * 	 	reply
 **********************************************************************************/
DBusMessage * npd_dbus_diagnosis_pci_hw_rw_reg(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	DBusMessage* reply;
	unsigned int ret = DIAG_RETURN_CODE_SUCCESS;
	unsigned int hwType = 0;
	unsigned int opType = 0;
	unsigned int opDevice = 0;
	unsigned int opRegaddr = 0;
	unsigned int regValue = 0;
	
	DBusError err;
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,
									DBUS_TYPE_UINT32, &hwType,
									DBUS_TYPE_UINT32, &opType,
									DBUS_TYPE_UINT32, &opDevice,
									DBUS_TYPE_UINT32, &opRegaddr,
									DBUS_TYPE_UINT32, &regValue,
									DBUS_TYPE_INVALID))) {
		npd_syslog_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
				npd_syslog_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
		}
		ret = DIAG_RETURN_CODE_ERROR;
	}
	/* read or write pci reg*/
	switch (hwType) {
		case 1:
			if (opType == 0) {
				ret = nam_asic_pci_read(opDevice, opRegaddr, &regValue);
			}
			else if (opType == 1) {
				ret = nam_asic_pci_write(opDevice, opRegaddr, regValue);
			}
			else {
				npd_syslog_err("ERROR: unit %d MII Addr 0x%#x: operate is not read/write\n",
					opDevice);
				ret = DIAG_RETURN_CODE_ERROR;
			}
			break;
		default:
			npd_syslog_err("ERROR: hardware is error\n",
				hwType);
			ret = DIAG_RETURN_CODE_ERROR;
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32, &ret,
 							 DBUS_TYPE_UINT32, &regValue,
							 DBUS_TYPE_INVALID);
	return reply;
}	

/**********************************************************************************
 * npd_dbus_diagnosis_cpu_hw_rw_reg
 *		read/write CPU configuration register
 *
 * 	INPUT:
 *		DBusConnection *conn, 
 *		DBusMessage *msg, 
 *		void *user_data
 *
 *	OUTPUT:
 * 		None.
 *
 *	RETURN:
 * 	 	reply
 **********************************************************************************/
DBusMessage * npd_dbus_diagnosis_cpu_hw_rw_reg(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	DBusMessage* reply = NULL;
	unsigned int ret = DIAG_RETURN_CODE_SUCCESS;
	unsigned int opType = 0;
	unsigned int opRegaddr = 0;
	unsigned int regValue = 0;
	
	DBusError err;
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,
									DBUS_TYPE_UINT32, &opType,
									DBUS_TYPE_UINT32, &opRegaddr,
									DBUS_TYPE_UINT32, &regValue,
									DBUS_TYPE_INVALID))) {
		npd_syslog_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
				npd_syslog_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
		}
		ret = DIAG_RETURN_CODE_ERROR;
	}
	/* read or write cpu reg*/
	if (opType == 0) {
		ret = nam_board_cpu_read_reg(opRegaddr, &regValue);
	}
	else if (opType == 1) {
		ret = nam_board_cpu_write_reg(opRegaddr, regValue);
	}
	else {
		npd_syslog_err("ERROR: cpu register operation is not read/write\n");
		ret = DIAG_RETURN_CODE_ERROR;
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32, &ret,
 							 DBUS_TYPE_UINT32, &regValue,
							 DBUS_TYPE_INVALID);
	return reply;
}	

/**********************************************************************************
 * npd_dbus_diagnosis_cpld_hw_rw_reg
 *		read/write cpld register
 *
 * 	INPUT:
 *		DBusConnection *conn, 
 *		DBusMessage *msg, 
 *		void *user_data
 *
 *	OUTPUT:
 * 		register value
 *
 *	RETURN:
 * 	 	reply
 **********************************************************************************/
DBusMessage * npd_dbus_diagnosis_cpld_hw_rw_reg(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	DBusMessage* reply;
	unsigned int ret = DIAG_RETURN_CODE_SUCCESS;
	unsigned int hwType = 0;
	unsigned int opType = 0;
	unsigned int opDevice = 0;
	unsigned int opRegaddr = 0;
	unsigned int regValue = 0;
	unsigned char vlaue = 0;
	int address = 0;
	unsigned char regdata = 0;
	
	DBusError err;
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,
									DBUS_TYPE_UINT32, &hwType,
									DBUS_TYPE_UINT32, &opType,
									DBUS_TYPE_UINT32, &opDevice,
									DBUS_TYPE_UINT32, &opRegaddr,
									DBUS_TYPE_UINT32, &regValue,
									DBUS_TYPE_INVALID))) {
		npd_syslog_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
				npd_syslog_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
		}
		ret = DIAG_RETURN_CODE_ERROR;
	}
	/* read or write cpld reg*/
	switch (hwType) {
		case 1:
			if (opType == 0) {
				address = opRegaddr;
				nbm_cpld_reg_read(address, &regdata);
				regValue = regdata;
				npd_syslog_dbg("diag cpld register %#x read vlaue %#x\n", address, regValue);
			}
			else if (opType == 1) {
				vlaue = regValue;
				address = opRegaddr;
				npd_syslog_dbg("diag cpld register %#x read vlaue %#x\n", address, vlaue);
				ret = nbm_cpld_reg_write(address, vlaue);
			}
			else {
				npd_syslog_err("ERROR: unit %d MII Addr 0x%#x: operate is not read/write\n",
					opDevice);
				ret = DIAG_RETURN_CODE_ERROR;
			}
			break;
		default:
			npd_syslog_err("ERROR: hardware is error\n",
				hwType);
			ret = DIAG_RETURN_CODE_ERROR;
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32, &ret,
							 DBUS_TYPE_UINT32, &regValue,
							 DBUS_TYPE_INVALID);
	return reply;
}	

/**********************************************************************************
 * npd_dbus_diagnosis_eeprom_hw_rw_reg
 *		read/write eeprom register
 *
 * 	INPUT:
 *		DBusConnection *conn, 
 *		DBusMessage *msg, 
 *		void *user_data
 *
 *	OUTPUT:
 * 		register value
 *
 *	RETURN:
 * 	 	reply
 **********************************************************************************/
DBusMessage * npd_dbus_diagnosis_eeprom_hw_rw_reg(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	DBusMessage* reply;
	unsigned int ret = DIAG_RETURN_CODE_SUCCESS;
	unsigned int opType = 0;			/* read */
	unsigned char twsi_channel = 0;	/* TWSI channel */
	unsigned int eeprom_addr = 0;		/* eeprom address  */
	unsigned int eeprom_type = 0;		/* eeprom type */
	unsigned int validOffset = 0;		/* whether the slave has offset (i.e. Eeprom  etc.), true: valid false: in valid */
	unsigned int moreThan256 = 0;		/* whether the ofset is bigger than 256, true: valid false: in valid */
	unsigned int regAddr = 0;			/* address of eeprom's register */
	unsigned char regValue = 0;		/* value of eeprom's register */
	
	DBusError err;
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,
									DBUS_TYPE_UINT32, &opType,
									DBUS_TYPE_BYTE,   &twsi_channel,
									DBUS_TYPE_UINT32, &eeprom_addr,
									DBUS_TYPE_UINT32, &eeprom_type,
									DBUS_TYPE_UINT32, &validOffset,
									DBUS_TYPE_UINT32, &moreThan256,
									DBUS_TYPE_UINT32, &regAddr,
									DBUS_TYPE_BYTE,   &regValue,
									DBUS_TYPE_INVALID))) {
		npd_syslog_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
				npd_syslog_err("%s raised: %s", err.name, err.message);
				dbus_error_free(&err);
		}
		ret = DIAG_RETURN_CODE_ERROR;
	}

	npd_syslog_dbg("nbm_eeprom_reg_%s twsi_channel %d eeprom_addr %#x eeprom_type %d validOffset %s moreThan256 %s regAddr %#x vlaue is %#x\n",
					opType ? "write" : "read",
					twsi_channel, eeprom_addr, eeprom_type,
					validOffset ? "true" : "false",
					moreThan256 ? "true" : "false",
					regAddr, regValue);
	/* read or write eeprom reg */
	if (opType == 0) {
		ret = nbm_eeprom_reg_read(twsi_channel, eeprom_addr, eeprom_type, validOffset, moreThan256, regAddr, &regValue);
		npd_syslog_dbg("nbm_eeprom_reg_read vlaue is %#x ret %d\n", regValue, ret);
	}
	else if (opType == 1) {
		ret = nbm_eeprom_reg_write(twsi_channel, eeprom_addr, eeprom_type, validOffset, moreThan256, regAddr, regValue);
		npd_syslog_dbg("nbm_eeprom_reg_write ret %d\n", ret);
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32, &ret,
							 DBUS_TYPE_UINT32, &regValue,
							 DBUS_TYPE_INVALID);
	return reply;
}	

/**********************************************************************************
 * npd_dbus_diagnosis_mac_hw_rw_reg
 *		read/write mac register
 *
 * 	INPUT:
 *		DBusConnection *conn, 
 *		DBusMessage *msg, 
 *		void *user_data
 *
 *	OUTPUT:
 * 		register value
 *
 *	RETURN:
 * 	 	reply
 **********************************************************************************/
DBusMessage * npd_dbus_diagnosis_mac_hw_rw_reg(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	DBusMessage* reply;
	unsigned int ret = DIAG_RETURN_CODE_SUCCESS;
	unsigned int hwType = 0;
	unsigned int opType = 0;
	unsigned int opDevice = 0;
	unsigned int opPort = 0;
	unsigned int opRegtype = 0;
	unsigned int regValue = 0;
	unsigned int dataret = 0;
	unimac_data_t *unimac = NULL;
	
	unimac = (struct unimac_data_s*)malloc(sizeof(struct unimac_data_s));
	memset(unimac, 0, sizeof(struct unimac_data_s));
	
	DBusError err;
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,
									DBUS_TYPE_UINT32, &hwType,
									DBUS_TYPE_UINT32, &opType,
									DBUS_TYPE_UINT32, &opDevice,
									DBUS_TYPE_UINT32, &opPort,
									DBUS_TYPE_UINT32, &opRegtype,
									DBUS_TYPE_UINT32, &regValue,
									DBUS_TYPE_INVALID))) {
		npd_syslog_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
				npd_syslog_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
		}
		ret = DIAG_RETURN_CODE_ERROR;
	}
	/* read or write mac reg*/
	switch (hwType) {
		case 1:
			if (opType == 0) {
				unimac->rw = 1;
				unimac->port = opPort;
				unimac->unit = opDevice;
				unimac->type = opRegtype;
				nam_unimac_data_check_br(unimac, &dataret);
				regValue = dataret;
				npd_syslog_dbg("nam_unimac_data_check_br  vlaue is %#x\n", regValue);
			}
			else if (opType == 1) {
				unimac->rw = 0;
				unimac->port = opPort;
				unimac->unit = opDevice;
				unimac->type = opRegtype;
				unimac->data= regValue;
				nam_unimac_data_check_br(unimac, &dataret);
				npd_syslog_dbg("nam_unimac_data_check_br regvlaue is %#x\n", regValue);
			}
			else {
				npd_syslog_err("ERROR: unit %d MII Addr 0x%#x: operate is not read/write\n",
					opDevice);
				ret = DIAG_RETURN_CODE_ERROR;
			}
			break;
		default:
			npd_syslog_err("ERROR: hardware is error\n",
				hwType);
			ret = DIAG_RETURN_CODE_ERROR;
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32, &ret,
							 DBUS_TYPE_UINT32, &regValue,
							 DBUS_TYPE_INVALID);
	free(unimac);
	unimac = NULL;  /* code optimize: Resource leak. zhangdi@autelan.com 2013-01-18 */
	return reply;
}

/**********************************************************************************
 * npd_dbus_diagnosisi_hw_rw_reg
 *		read/write register
 *
 * 	INPUT:
 *		DBusConnection *conn, 
 *		DBusMessage *msg, 
 *		void *user_data
 *
 *	OUTPUT:
 * 		register value
 *
 *	RETURN:
 * 	 	reply
 **********************************************************************************/
DBusMessage * npd_dbus_diagnosis_hw_rw_reg(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	DBusMessage* reply;
	unsigned int ret = DIAG_RETURN_CODE_SUCCESS;
	unsigned int hwType = 0;
	unsigned int opType = 0;
	unsigned int opDevice = 0;
	unsigned int opGroupID = 0;
	unsigned int opPort = 0;
	unsigned int regaddr = 0;
	unsigned int regValue = 0;
	unsigned int dataret = 0;
	/*test for add vlan udp rule*/
		
	DBusError err;
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,
									DBUS_TYPE_UINT32, &hwType,
									DBUS_TYPE_UINT32, &opType,
									DBUS_TYPE_UINT32, &opDevice,
									DBUS_TYPE_UINT32, &opGroupID,
									DBUS_TYPE_UINT32, &opPort,
									DBUS_TYPE_UINT32, &regaddr,
									DBUS_TYPE_UINT32, &regValue,
									DBUS_TYPE_INVALID))) {
		npd_syslog_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
				npd_syslog_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
		}
		ret = DIAG_RETURN_CODE_ERROR;
	}
	/* read or write  reg*/
	switch (hwType) {
		case 1:
			if (opType == 0) {
				ret = nam_read_reg(opDevice, opGroupID, regaddr, &dataret);
				regValue = dataret;
				npd_syslog_dbg("read unit %d group %d register %#x vlaue %#x\n", opDevice, opGroupID, regaddr, regValue);
			}
			else if (opType == 1) {
				ret = nam_write_reg(opDevice, opGroupID, regaddr, regValue);
				npd_syslog_dbg("write unit %d group %d register %#x vlaue %#x\n", opDevice, opGroupID, regaddr, regValue);
			}
			else {
				npd_syslog_err("ERROR: unit %d MII Addr 0x%#x: operate is not read/write\n",
					opDevice);
				ret = DIAG_RETURN_CODE_ERROR;
			}
			break;
		default:
			npd_syslog_err("ERROR: hardware is error\n",
				hwType);
			ret = DIAG_RETURN_CODE_ERROR;
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32, &ret,
							 DBUS_TYPE_UINT32, &regValue,
							 DBUS_TYPE_INVALID);
	return reply;
}
DBusMessage * npd_dbus_diagnosis_hw_cscd_port_prbs_test(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	DBusMessage* reply;
	int i = 0;
	unsigned int ret = DIAG_RETURN_CODE_SUCCESS;
	unsigned int devnum = 0;
	unsigned int portnum = 0;
	unsigned int opDevice = 0;
	unsigned int opDeviceType = 0;
	unsigned int Gpmem[4] = {0,0,0,0};
	unsigned int Gpres[4] = {0,0,0,0};
 	DBusError err;
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,
									DBUS_TYPE_UINT32, &opDevice,
									DBUS_TYPE_INVALID))) {
		npd_syslog_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
				npd_syslog_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
		}
		ret = DIAG_RETURN_CODE_ERROR;
	}
	else {
		for(i=0; i < asic_board->asic_cscd_port_cnt; i++)
        {
            devnum = asic_board->asic_cscd_ports[i].dev_num;
            portnum = asic_board->asic_cscd_ports[i].port_num;
			npd_syslog_dbg("get asic port dev: %d portnum %d!\n",devnum,portnum);
        	if(opDevice == devnum)
        	{
			    if(portnum/16 == 0)
                    Gpmem[0] |= (1 << (portnum % 16));
			    else if(portnum/16 == 1)
				    Gpmem[1] |= (1 << (portnum % 16));
			    else if(portnum/16 == 2)
				    Gpmem[2] |= (1 << (portnum % 16));
			    else if(portnum/16 == 3)
				    Gpmem[3] |= (1 << (portnum % 16));
			    else
			    {
				    npd_syslog_dbg("portnum exceed:lion portnum less 64!\n");
			    }
            }
        }
		for(i = 0; i < 4; i++)
		{
			npd_syslog_dbg("dev %d group %d Gpmembitmap: 0x%x\n",devnum,i,Gpmem[i]);
		}
	    ret = nam_cscd_port_prbs_test(opDevice, Gpmem, Gpres);
		for(i = 0; i < 4; i++)
		{
			npd_syslog_dbg("dev %d group %d Gpresbitmap: 0x%x\n",devnum,i,Gpres[i]);
		}
	}

	reply = dbus_message_new_method_return(msg);
	dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32, &ret,
							 DBUS_TYPE_UINT32, &Gpmem[0],
							 DBUS_TYPE_UINT32, &Gpmem[1],
							 DBUS_TYPE_UINT32, &Gpmem[2],
							 DBUS_TYPE_UINT32, &Gpmem[3],
							 DBUS_TYPE_UINT32, &Gpres[0],
							 DBUS_TYPE_UINT32, &Gpres[1],
							 DBUS_TYPE_UINT32, &Gpres[2],
							 DBUS_TYPE_UINT32, &Gpres[3],
							 DBUS_TYPE_INVALID);
	return reply;
}
/**********************************************************************************
 * npd_dbus_diagnosis_hw_prbs_test
 *		prbs test
 *
 * 	INPUT:
 *		DBusConnection *conn, 
 *		DBusMessage *msg, 
 *		void *user_data
 *
 *	OUTPUT:
 * 		result
 *
 *	RETURN:
 * 	 	reply
 **********************************************************************************/
DBusMessage * npd_dbus_diagnosis_hw_prbs_test(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	DBusMessage* reply;
	unsigned int ret = DIAG_RETURN_CODE_SUCCESS;
	unsigned int opDevice = 0;
	unsigned int opPort = 0;
	unsigned int result = 0;
		
	DBusError err;
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,
									DBUS_TYPE_UINT32, &opDevice,
									DBUS_TYPE_UINT32, &opPort,
									DBUS_TYPE_UINT32, &result,
									DBUS_TYPE_INVALID))) {
		npd_syslog_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
				npd_syslog_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
		}
		ret = DIAG_RETURN_CODE_ERROR;
	}
	else {
	    ret = nam_test_prbs(opDevice, opPort, &result);
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32, &ret,
							 DBUS_TYPE_UINT32, &result,
							 DBUS_TYPE_INVALID);
	return reply;
}
/**********************************************************************************
 *  npd_dbus_diagnosis_hw_port_mode_set
 *
 * 	INPUT:
 *		DBusConnection *conn, 
 *		DBusMessage *msg, 
 *		void *user_data
 *
 *	OUTPUT:
 * 		ret
 *
 *	RETURN:
 * 	 	reply
 **********************************************************************************/
DBusMessage * npd_dbus_diagnosis_hw_port_mode_set(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	DBusMessage* reply;
	unsigned int ret = DIAG_RETURN_CODE_SUCCESS;
	unsigned int opDevice = 0;
	unsigned int opPort = 0;
	unsigned int portMode = 0;
		
	DBusError err;
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,
									DBUS_TYPE_UINT32, &opDevice,
									DBUS_TYPE_UINT32, &opPort,
									DBUS_TYPE_UINT32, &portMode,
									DBUS_TYPE_INVALID))) {
		npd_syslog_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
				npd_syslog_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
		}
		ret = DIAG_RETURN_CODE_ERROR;
	}
	else {
	    ret = nam_set_port_mode(opDevice, opPort, portMode);
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32, &ret,
							 DBUS_TYPE_INVALID);
	return reply;
}
/**********************************************************************************
 * npd_dbus_diagnosis_dump_hw_field_tab
 *		dump fileld table(broadcom)
 *
 * 	INPUT:
 *		DBusConnection *conn, 
 *		DBusMessage *msg, 
 *		void *user_data
 *
 *	OUTPUT:
 * 		showStr
 *
 *	RETURN:
 * 	 	reply
 **********************************************************************************/
DBusMessage * npd_dbus_diagnosis_dump_hw_field_tab(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	DBusMessage* reply;
	DBusMessageIter  iter;
	unsigned int ret = DIAG_RETURN_CODE_SUCCESS;
	unsigned int opDevice = 0;
	unsigned int gnum = 0;
	char *showStr = NULL;
                   
	showStr = (char*)malloc(102400);
	if(NULL == showStr) {
		npd_syslog_dbg("Malloc failed!\n");
		return NULL;
	}
	memset(showStr, 0, 102400);

	DBusError err;
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,
									
									DBUS_TYPE_UINT32, &opDevice,
									DBUS_TYPE_UINT32, &gnum,
									DBUS_TYPE_INVALID))) {
		npd_syslog_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
				npd_syslog_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
		}
		ret = DIAG_RETURN_CODE_ERROR;
	}

	
	nam_field_dump_br(opDevice, gnum, showStr);

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_STRING,
									 &showStr);	
	free(showStr);
	showStr = NULL;
	return reply;
}	

/**********************************************************************************
 * npd_dbus_diagnosis_dump_hw_cosq_tab
 *		dump cosq table(broadcom)
 *
 * 	INPUT:
 *		DBusConnection *conn, 
 *		DBusMessage *msg, 
 *		void *user_data
 *
 *	OUTPUT:
 * 		showStr
 *
 *	RETURN:
 * 	 	reply
 **********************************************************************************/
DBusMessage * npd_dbus_diagnosis_dump_hw_cosq_tab(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	DBusMessage* reply;
	DBusMessageIter  iter;
	unsigned int ret = DIAG_RETURN_CODE_SUCCESS;
	unsigned int opDevice = 0;
	unsigned int portnum = 0;
	char *showStr = NULL;
				   
	showStr = (char*)malloc(102400);
	if(NULL == showStr) {
		npd_syslog_dbg("Malloc failed!\n");
		return NULL;
	}
	memset(showStr, 0, 102400);

	DBusError err;
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,									
									DBUS_TYPE_UINT32, &opDevice,
									DBUS_TYPE_UINT32, &portnum,
									DBUS_TYPE_INVALID))) {
		npd_syslog_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
				npd_syslog_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
		}
		ret = DIAG_RETURN_CODE_ERROR;
	}
	npd_syslog_dbg("unit is %d, portnum is %d \n", opDevice, portnum);
	nam_cosq_dump_br(opDevice, portnum, showStr);

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_STRING,
									 &showStr); 
	
	free(showStr);
	showStr = NULL;
	return reply;
}	

/**********************************************************************************
 * npd_dbus_diagnosis_dump_hw_cosq_tab
 *		dump  table(broadcom)
 *
 * 	INPUT:
 *		DBusConnection *conn, 
 *		DBusMessage *msg, 
 *		void *user_data
 *
 *	OUTPUT:
 * 		showStr
 *
 *	RETURN:
 * 	 	reply
 **********************************************************************************/
DBusMessage * npd_dbus_diagnosis_dump_hw_tab(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	DBusMessage* reply;
	unsigned int ret = DIAG_RETURN_CODE_SUCCESS;
	unsigned int opDevice = 0;
	unsigned int opBlock = 0;
	unsigned int tabIndex = 0;
	unsigned int opRegtype = 0;
	char *regtab = NULL;
	unsigned int regdata[72];
	int i = 0;
    	DBusMessageIter                      iter_array,iter = {0};
		
	regtab = (char*)malloc(1024);
	if(NULL == regtab) {
		npd_syslog_dbg("Malloc failed!\n");
		return NULL;
	}
	memset(regtab, 0, 1024);
	memset(regdata, 0, 72);

	DBusError err;
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,									
								DBUS_TYPE_UINT32, &opDevice,
								DBUS_TYPE_UINT32, &opBlock,
								DBUS_TYPE_UINT32, &tabIndex,
								DBUS_TYPE_UINT32, &opRegtype,
								DBUS_TYPE_INVALID))) {
		npd_syslog_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
				npd_syslog_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
		}
		ret = DIAG_RETURN_CODE_ERROR;
	}
	npd_syslog_dbg("unit is %d, opblock is %d, tabIndex is %d \n", opDevice, opBlock, tabIndex);
	ret = nam_dump_tab_br(opDevice, opRegtype, opBlock, tabIndex, regtab);
	npd_syslog_dbg("nam_dump_tab_br ret is %d \n", ret);
	memcpy(regdata, regtab, 72);		/*change order to little endian*/

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32,
								 &ret);
	dbus_message_iter_open_container (&iter,
									DBUS_TYPE_ARRAY,
									DBUS_STRUCT_BEGIN_CHAR_AS_STRING	 /*begin*/
									DBUS_TYPE_UINT32_AS_STRING
									DBUS_STRUCT_END_CHAR_AS_STRING,     /*end*/
									&iter_array);

	for(i=0;i<18;i++) {
		DBusMessageIter iter_struct;	
		dbus_message_iter_open_container (&iter_array,
										   DBUS_TYPE_STRUCT,
										   NULL,
										   &iter_struct);

		dbus_message_iter_append_basic(&iter_struct,
									  DBUS_TYPE_UINT32,
									  &regdata[17-i]);
				
		dbus_message_iter_close_container (&iter_array, &iter_struct);
	}
	dbus_message_iter_close_container (&iter, &iter_array);

	free(regtab);
	regtab = NULL;
	return reply;
}	

/**********************************************************************************
 * npd_dbus_diagnosis_show_hw_acl
 *		show acl (marvell)
 *
 * 	INPUT:
 *		DBusConnection *conn, 
 *		DBusMessage *msg, 
 *		void *user_data
 *
 *	OUTPUT:
 * 		showStr
 *
 *	RETURN:
 * 	 	reply
 **********************************************************************************/
DBusMessage * npd_dbus_diagnosis_show_hw_acl(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	DBusMessage* reply;
	DBusMessageIter  iter;
	unsigned int ret = DIAG_RETURN_CODE_SUCCESS;
	unsigned int rulesize = 0, aclindex= 0;
	char *showStr = NULL;
	
	showStr = (char*)malloc(102400);
	if(NULL == showStr) {
		npd_syslog_dbg("Malloc failed!\n");
		return NULL;
	}
	memset(showStr, 0, 102400);

	DBusError err;
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,
									
									DBUS_TYPE_UINT32, &rulesize,
									DBUS_TYPE_UINT32, &aclindex,
									DBUS_TYPE_INVALID))) {
		npd_syslog_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
				npd_syslog_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
		}
		ret = DIAG_RETURN_CODE_ERROR;
	}
	
	nam_acl_show_ma(aclindex, rulesize, showStr);

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_STRING,
									 &showStr); 
	free(showStr);
	showStr = NULL;
	return reply;
}

/**********************************************************************************
 * npd_dbus_diagnosis_xaui_phy_hw_rw_reg
 *		read/write xaui phy register
 *
 * 	INPUT:
 *		DBusConnection *conn, 
 *		DBusMessage *msg, 
 *		void *user_data
 *
 *	OUTPUT:
 * 		register value
 *
 *	RETURN:
 * 	 	reply
 **********************************************************************************/
DBusMessage * npd_dbus_diagnosis_xaui_phy_hw_rw_reg
(
	DBusConnection *conn,
	DBusMessage *msg,
	void *user_data
)
{
	DBusMessage* reply;
	unsigned int ret = DIAG_RETURN_CODE_SUCCESS;

	unsigned int opType = 0;		/* read */
	unsigned int opDevice = 0;
	unsigned short opPort = 0;
	unsigned char phyID = 0;
	unsigned char isExtPhy = 0;
	unsigned short opRegaddr = 0;
	unsigned char phyDve = 0;
	unsigned short regValue = 0xFFFF;
	
	DBusError err;
	dbus_error_init(&err);
	if (!(dbus_message_get_args(msg, &err,
								DBUS_TYPE_UINT32, &opType,
								DBUS_TYPE_UINT32, &opDevice,
								DBUS_TYPE_UINT16, &opPort,
								DBUS_TYPE_BYTE,  &phyID,
								DBUS_TYPE_BYTE,  &isExtPhy,
								DBUS_TYPE_UINT16, &opRegaddr,
								DBUS_TYPE_BYTE,  &phyDve,
								DBUS_TYPE_UINT16, &regValue,
								DBUS_TYPE_INVALID)))
	{
		syslog_ax_product_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_product_err("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		ret = DIAG_RETURN_CODE_ERROR;
	}

	syslog_ax_product_dbg("diag %s device %d port %d phyID %d %s Regaddr %#x phyDve %d regValue %#x on product %#x.\n", 
			opType ? "write" : "read",
			opDevice, opPort, phyID,
			isExtPhy ?  "external":"internal",
			opRegaddr, phyDve, regValue,
			PRODUCT_ID);
	/* read/write xaui phy reg*/
	if (opType == 0) {
		ret = nam_asic_auxi_phy_read(opDevice, opPort, phyID, isExtPhy, opRegaddr, phyDve, &regValue);
	}
	else if (opType == 1) {
		ret = nam_asic_auxi_phy_write(opDevice, opPort, phyID, isExtPhy, opRegaddr, phyDve, regValue);
	}
	else{
		 syslog_ax_product_err("ERROR: device %d port %d regaddr 0x%#x: operate is not read/write\n",
			  opDevice, opPort, opRegaddr);
		ret = DIAG_RETURN_CODE_ERROR;
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32, &ret,
 							 DBUS_TYPE_UINT16, &regValue,
							 DBUS_TYPE_INVALID);
	return reply;
}	

/**********************************************************************************
 * npd_dbus_show_mib_ge_xg
 *		show mib of ge/xg port
 *
 * 	INPUT:
 *		DBusConnection *conn, 
 *		DBusMessage *msg, 
 *		void *user_data
 *
 *	OUTPUT:
 * 		register value
 *
 *	RETURN:
 * 	 	reply
 **********************************************************************************/
DBusMessage * npd_dbus_show_mib_ge_xg
(
	DBusConnection *conn,
	DBusMessage *msg,
	void *user_data
)
{
	DBusMessage* reply;
	unsigned int ret = DIAG_RETURN_CODE_SUCCESS;

	unsigned int portType = 0;
	
	DBusError err;
	dbus_error_init(&err);
	if (!(dbus_message_get_args(msg, &err,
								DBUS_TYPE_UINT32, &portType,
								DBUS_TYPE_INVALID)))
	{
		syslog_ax_product_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_product_err("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		ret = DIAG_RETURN_CODE_ERROR;
	}

	syslog_ax_product_dbg("show mib information of %s port.\n", portType ? "xg" : "ge");

	if (portType == 0) {
		ret = nam_asic_show_mib_ge();
	}
	else if (portType == 1) {
		ret = nam_asic_show_mib_xg();
	}
	else{
		 syslog_ax_product_err("unknown port type %d\n", portType);
		ret = DIAG_RETURN_CODE_ERROR;
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32, &ret,
							 DBUS_TYPE_INVALID);
	return reply;
}	

DBusMessage * npd_dbus_diagnosis_hw_watchdog_control(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	DBusMessage* reply = NULL;
	unsigned int ret = DIAG_RETURN_CODE_SUCCESS;
	unsigned int enabled = SYSTEM_HARDWARE_WATCHDOG_ENABLE;
	
	DBusError err;
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,
									DBUS_TYPE_UINT32, &enabled,
									DBUS_TYPE_INVALID))) {
		npd_syslog_err("Unable to get input args in hw watchdog control");
		if (dbus_error_is_set(&err)) {
				npd_syslog_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
		}
		ret = DIAG_RETURN_CODE_ERROR;
	}

	ret = nbm_hardware_watchdog_control_set(enabled);
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32, &ret,
							 DBUS_TYPE_INVALID);
	return reply;
}	

DBusMessage * npd_dbus_diagnosis_hw_watchdog_timeout_op(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	DBusMessage* reply;
	unsigned int ret = DIAG_RETURN_CODE_SUCCESS;
	unsigned int timeout = 0, opType = 0;
	unsigned int enabled = SYSTEM_HARDWARE_WATCHDOG_DISABLE;
	
	DBusError err;
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,
									DBUS_TYPE_UINT32, &opType,
									DBUS_TYPE_UINT32, &timeout,
									DBUS_TYPE_INVALID))) {
		npd_syslog_err("Unable to get input args in hw watchdog timeout set");
		if (dbus_error_is_set(&err)) {
				npd_syslog_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
		}
		ret = DIAG_RETURN_CODE_ERROR;
	}

	/* Get operation */
	if(SYSTEM_HARDWARE_WATCHDOG_TIMEOUT_OP_GET == opType) {
		ret = nbm_hardware_watchdog_control_get(&enabled);
		if(!ret) {
			timeout = 0;
			ret = nbm_hardware_watchdog_timeout_get(&timeout);
			if(!ret) {
				npd_syslog_dbg("hardware watchdog %s timeout %d\n", \
						(SYSTEM_HARDWARE_WATCHDOG_ENABLE == enabled) ? "enabled" : "disabled", timeout);;
			}
		}
		
	}
	/* Set operation */
	else if(SYSTEM_HARDWARE_WATCHDOG_TIMEOUT_OP_SET == opType) {
		ret = nbm_hardware_watchdog_timeout_set(timeout);
		if(!ret) {
			timeout = 0;
			nbm_hardware_watchdog_control_get(&enabled);
			nbm_hardware_watchdog_timeout_get(&timeout);
		}
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32, &ret,
							 DBUS_TYPE_UINT32, &timeout,
							 DBUS_TYPE_UINT32, &enabled,
							 DBUS_TYPE_INVALID);
	return reply;
}

DBusMessage * npd_dubs_equipment_cpu_temperature_test
(
	DBusConnection *conn,
	DBusMessage *msg,
	void *user_data
)
{
	DBusMessage* reply;
	unsigned int ret = DIAG_RETURN_CODE_SUCCESS;

	unsigned int opType = 0;
	int master_remote_tempe;
	int master_inter_tempe;
	int slave_remote_tempe;
	int slave_inter_tempe;	
	int master_temp1_input;
	int master_temp2_input;
	int slave_temp1_input;
	int slave_temp2_input;
	
	DBusError err;
	dbus_error_init(&err);
	if (!(dbus_message_get_args(msg, &err,
								DBUS_TYPE_UINT32, &opType,
    							DBUS_TYPE_INT32,&master_inter_tempe,
      							DBUS_TYPE_INT32,&master_remote_tempe,
      							DBUS_TYPE_INT32,&slave_inter_tempe,
      							DBUS_TYPE_INT32,&slave_remote_tempe,									
								DBUS_TYPE_INVALID)))
	{
		syslog_ax_product_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) 
		{
			syslog_ax_product_err("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		ret = DIAG_RETURN_CODE_ERROR;
	}

	syslog_ax_product_dbg("show cpu temperature.\n");

	if (opType == 1) 
	{
    	if (((master_temp1_input=get_num_from_file(MASTER_TEMP1_INPUT)) >= 0)
    			&&((master_temp2_input=get_num_from_file(MASTER_TEMP2_INPUT)) >= 0))
    	{

    		master_inter_tempe = master_temp2_input/1000;
    		master_remote_tempe = master_temp1_input/1000;

    		if (((slave_temp1_input=get_num_from_file(SLAVE_TEMP1_INPUT)) >= 0)
    				&&((slave_temp2_input=get_num_from_file(SLAVE_TEMP2_INPUT)) >= 0))
    		{
    		slave_inter_tempe = slave_temp2_input/1000;
    		slave_remote_tempe = slave_temp1_input/1000;
    		}
    		else
    		{
    			syslog_ax_product_err("ax81_ac4x max1619 temperature read cpu temperature error! \n");	
    		}
    	}
    	/*if use the temperature mointor tmp421*/
    	else if (((slave_temp1_input=get_num_from_file(TMP421_SLAVE_TEMP1_INPUT)) >= 0)
    					&&((slave_temp2_input=get_num_from_file(TMP421_SLAVE_TEMP2_INPUT)) >= 0)) 
    	{
    		slave_inter_tempe = slave_temp2_input/1000;
    		slave_remote_tempe = slave_temp1_input/1000;
    		
    		if (((master_temp1_input=get_num_from_file(SLAVE_TEMP1_INPUT)) >= 0)
    					&&((master_temp2_input=get_num_from_file(SLAVE_TEMP2_INPUT)) >= 0))
    		{

    			master_inter_tempe = master_temp2_input/1000;
    			master_remote_tempe = master_temp1_input/1000;	
    			
    		}
    		else
    		{
    			syslog_ax_product_err("ax81_ac12c tmp421 temperature read cpu temperature error! \n");
    		}  		
    	}
    	else
    	{
    		syslog_ax_product_err("ax81_4x read cpu temperature error! \n");
    	}   		

	}
	else
	{
		syslog_ax_product_err("unknown operation type %d\n", opType);
		ret = DIAG_RETURN_CODE_ERROR;
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32, &ret,
						     DBUS_TYPE_INT32,&master_inter_tempe,
  							 DBUS_TYPE_INT32,&master_remote_tempe,
  							 DBUS_TYPE_INT32,&slave_inter_tempe,
  							 DBUS_TYPE_INT32,&slave_remote_tempe,							 
							 DBUS_TYPE_INVALID);
	return reply;
}	


DBusMessage *npd_dubs_equipment_lion1_trunk_test
(
	DBusConnection *conn,
	DBusMessage *msg,
	void *user_data
)
{
	DBusMessage* reply;
	unsigned int ret = DIAG_RETURN_CODE_SUCCESS;
	unsigned int opType = 0;
	unsigned int devNum = 0;
	DBusError err;
	dbus_error_init(&err);
	if (!(dbus_message_get_args(msg, &err,
								DBUS_TYPE_UINT32, &opType,
								DBUS_TYPE_INVALID)))
	{
		syslog_ax_product_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_product_err("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		ret = DIAG_RETURN_CODE_ERROR;
	}

	syslog_ax_product_dbg("8610:4x igh-performance %s.\n", opType ? "enable" : "disable");
	if (opType == 1) 
	{
        /*for slot5*/

		ret = nam_asic_trunk_ports_del(devNum,118,32);
		if(ret != RET_SUCCESS)
		{
			syslog_ax_board_err("AX81 del port 32 to trunk 118 error !\n");
		}
		ret = nam_asic_trunk_ports_add(devNum,116,32,1);
		if(ret != RET_SUCCESS)
		{
			syslog_ax_board_err("AX81 add port 32 to trunk 116 error !\n");
		}
		ret = nam_asic_trunk_ports_del(devNum,118,36);
    	if(ret != RET_SUCCESS)
    	{
    		syslog_ax_board_err("AX81 del port 36 to trunk 118 error !\n");
    	}
		ret = nam_asic_trunk_ports_add(devNum,116,36,1);
    	if(ret != RET_SUCCESS)
    	{
    		syslog_ax_board_err("AX81 add port 36 to trunk 116 error !\n");
    	}
    	ret = nam_asic_trunk_ports_del(devNum,118,40);
    	if(ret != RET_SUCCESS)
    	{
    		syslog_ax_board_err("AX81 del port 40 to trunk 118 error !\n");
    	}
    	ret = nam_asic_trunk_ports_add(devNum,116,40,1);
    	if(ret != RET_SUCCESS)
    	{
    		syslog_ax_board_err("AX81 add port 40 to trunk 116 error !\n");
    	}

        /*for slot6*/
		ret = nam_asic_trunk_ports_del(devNum,117,34);
		if(ret != RET_SUCCESS)
		{
			syslog_ax_board_err("AX81 del port 34 to trunk 117 error !\n");
		}
		ret = nam_asic_trunk_ports_add(devNum,115,34,1);
		if(ret != RET_SUCCESS)
		{
			syslog_ax_board_err("AX81 add port 34 to trunk 115 error !\n");
		}
		ret = nam_asic_trunk_ports_del(devNum,117,38);
    	if(ret != RET_SUCCESS)
    	{
    		syslog_ax_board_err("AX81 del port 38 to trunk 117 error !\n");
    	}
		ret = nam_asic_trunk_ports_add(devNum,115,38,1);
    	if(ret != RET_SUCCESS)
    	{
    		syslog_ax_board_err("AX81 add port 38 to trunk 115 error !\n");
    	}
    	ret = nam_asic_trunk_ports_del(devNum,117,42);
    	if(ret != RET_SUCCESS)
    	{
    		syslog_ax_board_err("AX81 del port 42 to trunk 117 error !\n");
    	}
    	ret = nam_asic_trunk_ports_add(devNum,115,42,1);
    	if(ret != RET_SUCCESS)
    	{
    		syslog_ax_board_err("AX81 add port 42 to trunk 115 error !\n");
    	}		
	}
	else if (opType == 0) 
	{
        syslog_ax_product_dbg("we do not support test disable.\n");
    }
	else
	{
		syslog_ax_product_err("unknown operation type %d\n", opType);
		ret = DIAG_RETURN_CODE_ERROR;
	}
	reply = dbus_message_new_method_return(msg);
	dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32, &ret,
							 DBUS_TYPE_INVALID);
	return reply;
}

DBusMessage *npd_dubs_equipment_1x12g12s_trunk_port_test
(
	DBusConnection *conn,
	DBusMessage *msg,
	void *user_data
)
{
	DBusMessage* reply;
	unsigned int ret = DIAG_RETURN_CODE_SUCCESS;
	unsigned int opType = 0;
	unsigned int vid = 4000;
	unsigned int devnum = 0;   /* no use,just for printf */
	DBusError err;
	dbus_error_init(&err);
	if (!(dbus_message_get_args(msg, &err,
								DBUS_TYPE_UINT32, &opType,
								DBUS_TYPE_INVALID)))
	{
		syslog_ax_product_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_product_err("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		ret = DIAG_RETURN_CODE_ERROR;
	}
	devnum = 0;	
	vid = 2000;
    ret = create_vlan(devnum,vid);
	if(0 != ret)
		{
		syslog_ax_board_err("create new vlan %d error.\n",vid);
		return ret;
		}
    ret = nam_asic_vlan_entry_ports_add(devnum, vid, 26, 1);  /* 2-26 */
	if(0 != ret)
		{
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret;		
		}	
	ret = nam_asic_vlan_entry_ports_add(devnum, vid, 27, 1);  /* 2-27 */
		if(0 != ret)
		{
		syslog_ax_board_err("add untagged ports to new vlan %d error.\n",vid);
		return ret; 	
		}	
	syslog_ax_product_dbg("xcat trunk test %s.\n", opType ? "1" : "0");
	if (opType == 0) 
	{
    	syslog_ax_product_dbg("we test Xcat port26 without trunk.\n");
		ret =nam_set_ethport_enable(0, 26, 1);
		if (0 != ret)
        {
            syslog_ax_board_dbg("enable 26 port on 1x12G12S error !\n");
        }
		ret = nam_set_ethport_enable(0, 27, 0);
		if (0 != ret)
        {
            syslog_ax_board_dbg("disable 27 port on 1x12G12S error !\n");
        }
		ret = nam_asic_trunk_port_endis(0,26,119,1);
		if (0 != ret)
        {
            syslog_ax_board_dbg("nam_asic_trunk_port_endis error !\n");
        }
		ret = nam_asic_trunk_port_endis(0,27,119,0);
		if (0 != ret)
        {
            syslog_ax_board_dbg("nam_asic_trunk_port_endis error !\n");
        }
		ret = nam_set_port_mac_mode(0,0x1);
		if (0 != ret)
        {
            syslog_ax_board_dbg("nam_set_port_mac_mode error !\n");
        }
		ret = nam_set_port_mac_mode(1,0x0);
		if (0 != ret)
        {
            syslog_ax_board_dbg("nam_set_port_mac_mode error !\n");
        }
	}
	else if (opType == 1) 
	{
		syslog_ax_product_dbg("we test Xcat port27 without trunk.\n");
		ret =nam_set_ethport_enable(0, 26, 0);
		if (0 != ret)
        {
            syslog_ax_board_dbg("disable 26 port on 1x12G12S error !\n");
        }
		ret =nam_set_ethport_enable(0, 27, 1);
		if (0 != ret)
        {
            syslog_ax_board_dbg("enable 27 port on 1x12G12S error !\n");
        }
		ret = nam_asic_trunk_port_endis(0,26,119,0);
		if (0 != ret)
        {
            syslog_ax_board_dbg("nam_asic_trunk_port_endis error !\n");
        }
		ret = nam_asic_trunk_port_endis(0,27,119,1);
		if (0 != ret)
        {
            syslog_ax_board_dbg("nam_asic_trunk_port_endis error !\n");
        }
				ret = nam_set_port_mac_mode(0,0x0);
		if (0 != ret)
        {
            syslog_ax_board_dbg("nam_set_port_mac_mode error !\n");
        }
		ret = nam_set_port_mac_mode(1,0x1);
		if (0 != ret)
        {
            syslog_ax_board_dbg("nam_set_port_mac_mode error !\n");
        }
    }
	else
	{
		syslog_ax_product_err("unknown operation type %d\n", opType);
		ret = DIAG_RETURN_CODE_ERROR;
	}
	reply = dbus_message_new_method_return(msg);
	dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32, &ret,
							 DBUS_TYPE_INVALID);
	return reply;
}



/**********************************************************************************
 * npd_dbus_equipment_test
 *		equipment test
 *
 * 	INPUT:
 *		DBusConnection *conn, 
 *		DBusMessage *msg, 
 *		void *user_data
 *
 *	OUTPUT:
 * 		register value
 *
 *	RETURN:
 * 	 	reply
 **********************************************************************************/
DBusMessage * npd_dbus_equipment_test
(
	DBusConnection *conn,
	DBusMessage *msg,
	void *user_data
)
{
	DBusMessage* reply;
	unsigned int ret = DIAG_RETURN_CODE_SUCCESS;

	unsigned int opType = 0;
	
	DBusError err;
	dbus_error_init(&err);
	if (!(dbus_message_get_args(msg, &err,
								DBUS_TYPE_UINT32, &opType,
								DBUS_TYPE_INVALID)))
	{
		syslog_ax_product_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_product_err("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		ret = DIAG_RETURN_CODE_ERROR;
	}

	syslog_ax_product_dbg("ge_xg port test %s.\n", opType ? "on" : "off");

	if (opType == 0) {
    	syslog_ax_product_dbg("we do not support test off.\n");
	}
	else if (opType == 1) {
		ret = equipment_test();   /* npd_board.c */
	}
	else{
		 syslog_ax_product_err("unknown operation type %d\n", opType);
		ret = DIAG_RETURN_CODE_ERROR;
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32, &ret,
							 DBUS_TYPE_INVALID);
	return reply;
}	


/**********************************************************************************
 * npd_dbus_ax8610_prbs_test
 *
 * 	INPUT:
 *		DBusConnection *conn, 
 *		DBusMessage *msg, 
 *		void *user_data
 *
 *	OUTPUT:
 * 		register value
 *
 *	RETURN:
 * 	 	reply
 **********************************************************************************/
DBusMessage * npd_dbus_hw_test_ax8610_prbs
(
	DBusConnection *conn,
	DBusMessage *msg,
	void *user_data
)
{
	DBusMessage* reply;
	unsigned int ret = DIAG_RETURN_CODE_SUCCESS;
    unsigned int slotBitMap = 0;
	unsigned int opType = 0;
	
	DBusError err;
	dbus_error_init(&err);
	if (!(dbus_message_get_args(msg, &err,
								DBUS_TYPE_UINT32, &opType,
								DBUS_TYPE_INVALID)))
	{
		syslog_ax_product_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_product_err("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		ret = DIAG_RETURN_CODE_ERROR;
	}

	syslog_ax_product_dbg("AX8610 PRBS test Lion%d...\n", opType);

    ret = npd_hw_diag_ax8610_prbs_test(opType, &slotBitMap);

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32, &ret,
							 DBUS_TYPE_UINT32, &slotBitMap,
							 DBUS_TYPE_INVALID);
	return reply;
}	



/**********************************************************************************
 * npd_dbus_mcb_state_change_test
 *		master state change test
 *
 * 	INPUT:
 *		DBusConnection *conn, 
 *		DBusMessage *msg, 
 *		void *user_data
 *
 *	OUTPUT:
 * 		register value
 *
 *	RETURN:
 * 	 	reply
 **********************************************************************************/
DBusMessage * npd_dbus_mcb_state_change_test
(
	DBusConnection *conn,
	DBusMessage *msg,
	void *user_data
)
{
	DBusMessage* reply;
	unsigned int ret = DIAG_RETURN_CODE_SUCCESS;

	unsigned int opType = 0;
	unsigned int i = 0;
	int devnum = 0;
	int portnum = 0;
	int eth_g_index = 0;
	
	DBusError err;
	dbus_error_init(&err);
	if (!(dbus_message_get_args(msg, &err,
								DBUS_TYPE_UINT32, &opType,
								DBUS_TYPE_INVALID)))
	{
		syslog_ax_product_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_product_err("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		ret = DIAG_RETURN_CODE_ERROR;
	}
	
	if(BOARD_TYPE != BOARD_TYPE_AX81_SMU)
	{
        for(i=0; i < asic_board->asic_cscd_port_cnt; i++)
        {
        	devnum = asic_board->asic_cscd_ports[i].dev_num;
        	portnum = asic_board->asic_cscd_ports[i].port_num;
			#if 1 
        	ret = npd_get_global_index_by_devport(devnum,portnum,&eth_g_index);
			if(0 != ret)
			{
    			ret = DIAG_RETURN_CODE_ERROR;				
                syslog_ax_board_err ("Fail to get gloable index when delete fdb one!/n");
			}
			
            ret =nam_fdb_table_delete_entry_with_port(eth_g_index);
            if(0 != ret)
            {
    			ret = DIAG_RETURN_CODE_ERROR;				
                syslog_ax_board_err("delete fdb of port (%d,%d) fail !\n",devnum,portnum);
            }
            syslog_ax_board_dbg("delete fdb of port  (%d,%d) OK\n",devnum, portnum);
            #endif
        }	
	}
	else
	{
        /* Enable the cscd port on Standby-MCB on AX8610  */	
    	if(0 == IS_ACTIVE_MASTER_NPD)    /* if local board is not Active-MCB */
    	{
       		ret = npd_enable_cscd_port();
            if (0 != ret)
            {
    			ret = DIAG_RETURN_CODE_ERROR;
                syslog_ax_netlink_info("enable cscd port on Standby-MCB error !\n");
            }
    		else
    		{
    			board_info.is_active_master = 1;
    	        syslog_ax_netlink_info("enable cscd port on Standby-MCB OK !\n");
    		}		
    	}
    	else    /* Disable the cscd port on Standby-MCB on AX8610  */
    	{
       		ret = npd_disable_cscd_port();
            if (0 != ret)
            {
    			ret = DIAG_RETURN_CODE_ERROR;			
                syslog_ax_netlink_info("disable cscd port on Active-MCB error !\n");
            }
    		else
    		{
    			board_info.is_active_master = 0;	
    	        syslog_ax_netlink_info("disable cscd port on Active-MCB OK !\n");
    		}
    		
    	}
	}
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32, &ret,
							 DBUS_TYPE_INVALID);
	return reply;
}	

/**********************************************************************************
 * npd_dbus_diagnosis_create_vlan
 *		create vlan for diagnosis
 *
 * 	INPUT:
 *		DBusConnection *conn, 
 *		DBusMessage *msg, 
 *		void *user_data
 *
 *	OUTPUT:
 * 		return value of create vlan
 *
 *	RETURN:
 * 	 	reply
 **********************************************************************************/
DBusMessage * npd_dbus_diagnosis_create_vlan
(
	DBusConnection *conn,
	DBusMessage *msg,
	void *user_data
)
{
	DBusMessage* reply;
	unsigned int ret = DIAG_RETURN_CODE_SUCCESS;

	unsigned int devnum = 0;
	unsigned int vlanId = 0;
	
	DBusError err;
	dbus_error_init(&err);
	if (!(dbus_message_get_args(msg, &err,
								DBUS_TYPE_UINT32, &devnum,
								DBUS_TYPE_UINT32, &vlanId,

								DBUS_TYPE_INVALID)))
	{
		syslog_ax_board_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_board_err("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		ret = DIAG_RETURN_CODE_ERROR;
	}
	ret = create_vlan(devnum,vlanId);	 /* create it! */			 
	if(0 != ret)
	{
		ret = DIAG_RETURN_CODE_ERROR;				
        syslog_ax_board_err("Fail to create vlan %d !/n",vlanId);
	}
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32, &ret,
							 DBUS_TYPE_INVALID);
	return reply;
}	
DBusMessage * npd_dbus_diagnosis_show_cscd_port_status(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusMessageIter	 iter_array;
	DBusError err;
	dbus_error_init(&err);
	unsigned int ret = 0;
	int i = 0,cscdportcnt=0;
	unsigned char devNum = 0,port = 0;
	unsigned long portstatus = 0,portendis = 0,member = 0;
	unsigned short trunkId = 0;
	cscdportcnt= asic_board->asic_cscd_port_cnt;
	
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,&cscdportcnt);
	dbus_message_iter_open_container (&iter,
									   	DBUS_TYPE_ARRAY,
									   	DBUS_STRUCT_BEGIN_CHAR_AS_STRING
										DBUS_TYPE_BYTE_AS_STRING
										DBUS_TYPE_BYTE_AS_STRING
										DBUS_TYPE_UINT32_AS_STRING
										DBUS_TYPE_UINT32_AS_STRING
										DBUS_TYPE_UINT16_AS_STRING
									   	DBUS_STRUCT_END_CHAR_AS_STRING,
									   &iter_array);
	
	for(i=0; i< cscdportcnt; i++)
	{

		devNum = asic_board->asic_cscd_ports[i].dev_num;
		port = asic_board->asic_cscd_ports[i].port_num;
		ret = nam_get_port_link_state(devNum,port,&portstatus);
		if(ret != NPD_SUCCESS)
		{
			syslog_ax_board_err("get %d port %d status failed \n",asic_board->asic_cscd_ports[i].dev_num,asic_board->asic_cscd_ports[i].port_num);
			return NPD_FAIL;
		}
		ret = nam_get_port_en_dis(devNum,port,&portendis);
		if(ret != NPD_SUCCESS)
		{
			syslog_ax_board_err("get (%d,%d) endis failed\n",devNum,port);
		}
		ret = nam_trunk_port_trunkId_get(devNum,port,&member,&trunkId);
		if(ret != NPD_SUCCESS)
		{
			syslog_ax_board_err("get (%d,%d) trunk Id failed\n",devNum,port);
		}
		if(member == 0)
		{
			trunkId = 0;/*did not a trunk port*/
		}
		syslog_ax_board_dbg("(%d,%d) is linkstatus %s  endis %s trunk id -> %d\n",devNum,port,portstatus?"UP":"DOWN",portendis?"En":"Dis",trunkId);
		DBusMessageIter iter_struct;
		dbus_message_iter_open_container (&iter_array,
											   DBUS_TYPE_STRUCT,
											   NULL,
											   &iter_struct);
		dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &devNum);
		dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &port);
		dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &portstatus);
		dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &portendis);
		dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT16,
					 &trunkId);
		dbus_message_iter_close_container (&iter_array, &iter_struct);
	}
	dbus_message_iter_close_container (&iter, &iter_array);
	
	
return reply;
}

DBusMessage * npd_dbus_diagnosis_vlan_fdb_delete(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	unsigned int	ret = 0;
	DBusError err;
	unsigned short vlanid;
	dbus_error_init(&err);
	syslog_ax_board_dbg("%s,%s,%d\n",__FILE__,__FUNCTION__,__LINE__);
	if (!(dbus_message_get_args ( msg, &err,
											DBUS_TYPE_UINT16,&vlanid,
											DBUS_TYPE_INVALID))) 
	{
		syslog_ax_board_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
				syslog_ax_board_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
		}
		return NULL;
	}
	ret =  nam_fdb_table_delete_entry_with_vlan_for_debug(vlanid);
	if (ret != DIAG_RETURN_CODE_SUCCESS) 
	{
		syslog_ax_board_err("nam_fdb_table_delete_entry_with_vlan_for_debug:erro !\n");
		ret = DIAG_RETURN_CODE_ERROR;
	}
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	
	
return reply;
}


/**********************************************************************************
 * npd_dbus_diagnosis_vlan_add_del_port
 *		add/del port to vlan
 *
 * 	INPUT:
 *		DBusConnection *conn, 
 *		DBusMessage *msg, 
 *		void *user_data
 *
 *	OUTPUT:
 * 		return value 
 *
 *	RETURN:
 * 	 	reply
 **********************************************************************************/
DBusMessage * npd_dbus_diagnosis_vlan_add_del_port
(
	DBusConnection *conn,
	DBusMessage *msg,
	void *user_data
)
{
	DBusMessage* reply;
	unsigned int ret = DIAG_RETURN_CODE_SUCCESS;
	unsigned char	isAdd = 0, isTagged = 0;
	
	unsigned int devnum = 0;
	unsigned int portnum = 0;
	unsigned int vlanId = 0;
	unsigned int needJoin = 0;
	
	DBusError err;
	dbus_error_init(&err);
	if (!(dbus_message_get_args(msg, &err,
							 	DBUS_TYPE_BYTE,&isAdd,
								DBUS_TYPE_UINT32,&devnum,
								DBUS_TYPE_UINT32,&portnum,								
							 	DBUS_TYPE_UINT32,&vlanId,								
							 	DBUS_TYPE_BYTE,&isTagged,
							 	DBUS_TYPE_INVALID)))
	{
		syslog_ax_board_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_board_err("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
    if(1==isAdd)
    {
    	ret = nam_asic_vlan_entry_ports_add(devnum, vlanId, portnum, isTagged);
        if(0 != ret)
    	{
    		syslog_ax_board_err("nam add port (%d,%d) %s to vlan %d error\n",devnum, portnum,isTagged?"tag":"untag",vlanId);
    		ret = DIAG_RETURN_CODE_ERROR;				
        }
    	syslog_ax_board_dbg("nam add port (%d,%d) %s to vlan %d OK\n",devnum, portnum,isTagged?"tag":"untag",vlanId);
    }
	else   /* delete port */
	{
		if(0==isTagged)
		{
			needJoin = 1;
		}
    	ret = nam_asic_vlan_entry_ports_del(vlanId, devnum , portnum, needJoin);
        if(0 != ret)
		{
    		syslog_ax_board_err("asic_board delete port %d from vlan %d error\n",portnum,vlanId);
    		ret = DIAG_RETURN_CODE_ERROR;				
	    }
	    syslog_ax_board_dbg("delete devNum %d,PortNum %d from vlan %d  %s OK.\n",devnum,portnum,vlanId,isTagged?"tag":"untag");
	}
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32, &ret,
							 DBUS_TYPE_INVALID);
	return reply;
}	

/**********************************************************************************
 * npd_dbus_diagnosis_show_port_mib
 * 
 * DESCRIPTION:
 *	This method get port mib information
 *
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		
 *	zhangdi@autelan.com 2012-04-24	
 **********************************************************************************/
DBusMessage * npd_dbus_diagnosis_show_port_mib
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;

	/* ASIC_PORT_MAC_MIB_S portMacMib;*/
	ASIC_PORT_MAC_MIB_DIAG_S portMacMib;    /* use the new struct for more info */
	unsigned char devNum = 0;
	unsigned char portNum = 0;
	unsigned int ret = DIAG_RETURN_CODE_SUCCESS;
	
	memset(&portMacMib, 0,sizeof(ASIC_PORT_MAC_MIB_DIAG_S));
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,
								DBUS_TYPE_BYTE,&devNum,
								DBUS_TYPE_BYTE,&portNum,			
							    DBUS_TYPE_INVALID)))
	{
		syslog_ax_board_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_board_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	reply = dbus_message_new_method_return(msg);	
	dbus_message_iter_init_append (reply, &iter);

    /* read mac mib */
	ret = nam_asic_get_port_mac_mib(devNum,portNum,&portMacMib); 
	if(0 != ret){
		syslog_ax_board_err("get dev %d port %d mib error.\n", devNum, portNum);
	}
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.goodOctetsRcv[0]));
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.goodOctetsRcv[1]));

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.badOctetsRcv[0]));
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.badOctetsRcv[1]));

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.macTransmitErr[0]));
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.macTransmitErr[1]));

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.goodPktsRcv[0]));
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.goodPktsRcv[1]));

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.badPktsRcv[0]));
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.badPktsRcv[1]));

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.brdcPktsRcv[0]));
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.brdcPktsRcv[1]));

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.mcPktsRcv[0]));
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.mcPktsRcv[1]));

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.pkts64Octets[0]));
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.pkts64Octets[1]));

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.pkts65to127Octets[0]));
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.pkts65to127Octets[1]));

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.pkts128to255Octets[0]));
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.pkts128to255Octets[1]));

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.pkts256to511Octets[0]));
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.pkts256to511Octets[1]));

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.pkts512to1023Octets[0]));
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.pkts512to1023Octets[1]));
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.pkts1024tomaxOoctets[0]));
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.pkts1024tomaxOoctets[1]));

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.goodOctetsSent[0]));
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.goodOctetsSent[1]));

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.goodPktsSent[0]));
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.goodPktsSent[1]));

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.excessiveCollisions[0]));
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.excessiveCollisions[1]));
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.mcPktsSent[0]));
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.mcPktsSent[1]));

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.brdcPktsSent[0]));
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.brdcPktsSent[1]));

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.unrecogMacCntrRcv[0]));
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.unrecogMacCntrRcv[1]));

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.fcSent[0]));
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.fcSent[1]));
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.goodFcRcv[0]));
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.goodFcRcv[1]));

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.dropEvents[0]));
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.dropEvents[1]));

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.undersizePkts[0]));
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.undersizePkts[1]));

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.fragmentsPkts[0]));
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.fragmentsPkts[1]));	

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.oversizePkts[0]));
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.oversizePkts[1]));

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.jabberPkts[0]));
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.jabberPkts[1]));

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.macRcvError[0]));
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.macRcvError[1]));

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.badCrc[0]));
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.badCrc[1]));
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.collisions[0]));
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.collisions[1]));

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.lateCollisions[0]));
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.lateCollisions[1]));

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.badFcRcv[0]));
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.badFcRcv[1]));

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.ucPktsRcv[0]));
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.ucPktsRcv[1]));
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.ucPktsSent[0]));
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.ucPktsSent[1]));

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.multiplePktsSent[0]));
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.multiplePktsSent[1]));

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.deferredPktsSent[0]));
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&(portMacMib.deferredPktsSent[1]));
		
	#if 0
	dbus_message_iter_append_basic (&iter,
						DBUS_TYPE_UINT32,&(portMacMib.rx.goodPkts[0]));
	dbus_message_iter_append_basic (&iter,
						DBUS_TYPE_UINT32,&(portMacMib.rx.goodPkts[1]));
	dbus_message_iter_append_basic (&iter,
						DBUS_TYPE_UINT32,&(portMacMib.rx.badPkts[0]));
	dbus_message_iter_append_basic (&iter,
						DBUS_TYPE_UINT32,&(portMacMib.rx.badPkts[1]));				
	dbus_message_iter_append_basic (&iter,
						DBUS_TYPE_UINT32,&(portMacMib.rx.goodOctets[0]));
	dbus_message_iter_append_basic (&iter,
						DBUS_TYPE_UINT32,&(portMacMib.rx.goodOctets[1]));				
	dbus_message_iter_append_basic (&iter,
						DBUS_TYPE_UINT32,&(portMacMib.rx.badOctets[0]));
	dbus_message_iter_append_basic (&iter,
						DBUS_TYPE_UINT32,&(portMacMib.rx.badOctets[1]));				
	dbus_message_iter_append_basic (&iter,
						DBUS_TYPE_UINT32,&(portMacMib.rx.internalDrop[0]));
	dbus_message_iter_append_basic (&iter,
						DBUS_TYPE_UINT32,&(portMacMib.rx.internalDrop[1]));	
	
	dbus_message_iter_append_basic (&iter,
						DBUS_TYPE_UINT32,&(portMacMib.rx.goodBCpkts[0]));
	dbus_message_iter_append_basic (&iter,
						DBUS_TYPE_UINT32,&(portMacMib.rx.goodBCpkts[1]));	

	dbus_message_iter_append_basic (&iter,
						DBUS_TYPE_UINT32,&(portMacMib.rx.goodUCpkts[0]));
	dbus_message_iter_append_basic (&iter,
						DBUS_TYPE_UINT32,&(portMacMib.rx.goodUCpkts[1]));	
	/* tx */
	dbus_message_iter_append_basic (&iter,
						DBUS_TYPE_UINT32,&(portMacMib.tx.goodPkts[0]));
	dbus_message_iter_append_basic (&iter,
						DBUS_TYPE_UINT32,&(portMacMib.tx.goodPkts[1]));				
	dbus_message_iter_append_basic (&iter,
						DBUS_TYPE_UINT32,&(portMacMib.tx.goodOctets[0]));
	dbus_message_iter_append_basic (&iter,
						DBUS_TYPE_UINT32,&(portMacMib.tx.goodOctets[1]));				
	dbus_message_iter_append_basic (&iter,
						DBUS_TYPE_UINT32,&(portMacMib.tx.macErrPkts[0]));
	dbus_message_iter_append_basic (&iter,
						DBUS_TYPE_UINT32,&(portMacMib.tx.macErrPkts[1]));				

	dbus_message_iter_append_basic (&iter,
						DBUS_TYPE_UINT32,&(portMacMib.tx.goodBCpkts[0]));	
	dbus_message_iter_append_basic (&iter,
						DBUS_TYPE_UINT32,&(portMacMib.tx.goodBCpkts[1]));
	
	dbus_message_iter_append_basic (&iter,
						DBUS_TYPE_UINT32,&(portMacMib.tx.goodUCpkts[0]));	
	dbus_message_iter_append_basic (&iter,
						DBUS_TYPE_UINT32,&(portMacMib.tx.goodUCpkts[1]));
	#endif		
	return reply;
}

/**********************************************************************************
 * npd_dbus_ethports_interface_get_port_rate
 * 
 * DESCRIPTION:
 *	This method get port mib information
 *
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		
 *	houxx@autelan.com 2013-03-8	
 **********************************************************************************/
DBusMessage * npd_dbus_ethports_interface_get_port_rate
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;

	/* ASIC_PORT_MAC_MIB_S portMacMib;*/
	ASIC_PORT_MAC_MIB_DIAG_S portMacMib;    /* use the new struct for more info */
	unsigned char devNum = 0;
	unsigned char portNum = 0;
	unsigned char slot_no = 0,port_no = 0; 
	unsigned int ret = DIAG_RETURN_CODE_SUCCESS;
	unsigned char type = 0;
	unsigned int port_index = 0;
	int i = 0;
	int eth_g_index = 0;
	
	unsigned long long bitsumrecv = 0,packetsumrecv = 0;
	unsigned long long bitsumsent = 0,packetsumsent = 0,power = 0;
	
	memset(&portMacMib, 0,sizeof(ASIC_PORT_MAC_MIB_DIAG_S));
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,
								DBUS_TYPE_BYTE,&slot_no,
								DBUS_TYPE_BYTE,&port_no,		
								DBUS_TYPE_BYTE,&type,
								DBUS_TYPE_UINT32,&port_index,
							    DBUS_TYPE_INVALID)))
	{
		syslog_ax_board_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_board_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	if(0 == type)
	{
		devNum = slot_no;
		portNum = port_no;
	}
	else
	{
		eth_g_index = port_index - (asic_board->asic_port_start_no -1);			
		ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
		if(ret != 0)
		{
			syslog_ax_board_err("npd_get_devport_by_global_index  FAIL. \n");
			return NULL;
		}
		
	}
	
	reply = dbus_message_new_method_return(msg);	
	dbus_message_iter_init_append (reply, &iter);

    /* read mac mib */
	
	for(i = 0;i<2; i++) /*times +1*/
	{
		ret = nam_asic_get_port_mac_mib(devNum,portNum,&portMacMib); 
		if(0 != ret){
			syslog_ax_board_err("get dev %d port %d mib error.\n", devNum, portNum);
			return NULL;
		}
		
		if(i == 0)
			sleep(3);
	}
	
	power = pow(10,10);
	bitsumrecv = portMacMib.goodOctetsRcv[1]*power + portMacMib.goodOctetsRcv[0] + portMacMib.goodOctetsRcv[3]*power + portMacMib.goodOctetsRcv[2];
	packetsumrecv = portMacMib.goodOctetsRcv[7]*power + portMacMib.goodOctetsRcv[6] + portMacMib.goodOctetsRcv[9]*power +portMacMib.goodOctetsRcv[8] + portMacMib.goodOctetsRcv[62] + portMacMib.goodOctetsRcv[63]*power + portMacMib.goodOctetsRcv[10] + portMacMib.goodOctetsRcv[11]*power + portMacMib.goodOctetsRcv[13]*power +portMacMib.goodOctetsRcv[12];
	bitsumsent = portMacMib.goodOctetsRcv[27]*power + portMacMib.goodOctetsRcv[26];
	packetsumsent = portMacMib.goodOctetsRcv[29]*power + portMacMib.goodOctetsRcv[28] + portMacMib.goodOctetsRcv[65]*power +portMacMib.goodOctetsRcv[64] + portMacMib.goodOctetsRcv[35]*power + portMacMib.goodOctetsRcv[34]+ portMacMib.goodOctetsRcv[33]*power + portMacMib.goodOctetsRcv[32];


	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT64,&bitsumrecv);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT64,&packetsumrecv);

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT64,&bitsumsent);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT64,&packetsumsent);


	return reply;
}

DBusMessage * npd_dbus_diagnosis_endis_asic
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;

	unsigned char asic_num = 0;
	unsigned char endis = 0;
	unsigned int ret = DIAG_RETURN_CODE_SUCCESS;
	
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,
								DBUS_TYPE_BYTE,&asic_num,
								DBUS_TYPE_BYTE,&endis,			
							    DBUS_TYPE_INVALID)))
	{
		syslog_ax_board_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) 
		{
			syslog_ax_board_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	/*endis asic*/
	ret = nam_enable_device(asic_num,endis);
	if(0 != ret)
	{
		syslog_ax_board_err("%s asic %s fail !!!\n",endis?"Enable":"Disable",asic_num);
	}
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32, &ret,
							 DBUS_TYPE_INVALID);

	return reply;
}

DBusMessage * npd_dbus_diagnosis_port_cscd_mode_set
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{	
	DBusMessage* reply;
	DBusMessageIter  iter;

	unsigned char	isCscd = 0;
	unsigned char	slot_no = 0, local_port_no = 0, slot_index = 0, local_port_index = 0;
	unsigned int	eth_g_index = 0;
	unsigned int	ret = DIAG_RETURN_CODE_SUCCESS;
	unsigned char   devnum = 0,portnum = 0;

	DBusError err;	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
							DBUS_TYPE_BYTE,&isCscd,
							DBUS_TYPE_BYTE,&slot_no,
							DBUS_TYPE_BYTE,&local_port_no,
							DBUS_TYPE_INVALID))) 
	{
		syslog_ax_trunk_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) 
		{
			syslog_ax_trunk_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}	
	syslog_ax_board_err("isCscd = %d,slot_no = %d,local_port_no = %d\n",isCscd,slot_no,local_port_no);
	if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) 
	{
		if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,local_port_no)) 
		{
			slot_index = CHASSIS_SLOT_NO2INDEX(slot_no);
			local_port_index = ETH_LOCAL_NO2INDEX(slot_index, local_port_no);			
			eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(slot_index,local_port_index);
			ret = npd_get_devport_by_global_index(eth_g_index,&devnum,&portnum);
			if(DIAG_RETURN_CODE_SUCCESS == ret) 
			{
				ret = force_network_port_to_cscd_mode(devnum,portnum,isCscd);
				if(ret != DIAG_RETURN_CODE_SUCCESS)
				{
					syslog_ax_board_err("set port %d/%d to %s mode fail !\n",slot_no,local_port_no,isCscd?"cscd":"network");
					ret = DIAG_RETURN_CODE_ERROR;
				}
			}
		}
		else 
		{
			ret = DIAG_RETURN_CODE_ERROR;
		}
	}
	else 
	{
		ret = DIAG_RETURN_CODE_ERROR;
	}
		


	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,&ret);

	return reply;
}
DBusMessage * npd_dbus_diagnosis_board_test_mode_set
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{	
	DBusMessage* reply;
	DBusMessageIter  iter;

	unsigned char	endis = 0;
	unsigned int	ret = DIAG_RETURN_CODE_SUCCESS;
	unsigned char   devnum = 0;

	DBusError err;	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
							DBUS_TYPE_BYTE,&endis,
							DBUS_TYPE_INVALID))) 
	{
		syslog_ax_board_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) 
		{
			syslog_ax_board_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}	
	syslog_ax_board_dbg("endis = %d,\n",endis);

	ret = npd_board_4x_12c_test(devnum,endis);
	if(ret != DIAG_RETURN_CODE_SUCCESS)
	{
		syslog_ax_board_err("set system to %s mode failed\n",endis?"test":"default");
	}

	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,&ret);

	return reply;
}

#ifdef __cplusplus
}
#endif


