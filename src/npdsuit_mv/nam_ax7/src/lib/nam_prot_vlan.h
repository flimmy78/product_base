#ifndef __NAM_PROT_VLAN_H__
#define __NAM_PROT_VLAN_H__

/*************************************************************
 * DESCRIPTION:
 *              this function get ether type and isvalid info by entry 
 *  INPUT:
 *              devNum - uint8 :  dev num
 *              entryNum - unit32 : the entry who's info be want
 * OUTPUT:
 *              etherType  - uint16 : ether type of this entry
 *              isValid   - uint 32 : is this entry valid 
 *                                                       TRUE  - isValid
 *                                                       FALSE - isInvalid
 * RETURN:
 *              PROTOCOL_VLAN_RETURN_CODE_SUCCESS    -  operation success
 *              PROTOCOL_VLAN_RETURN_CODE_NAM_ERR    -   operation failed
 * NOTE:
 *
 *************************************************************/
int nam_prot_vlan_get_entry_eth_type
(
    unsigned char   devNum,
    unsigned int    entryNum,
    unsigned short *etherType,
    unsigned int   *isValid
);

/*************************************************************
 * DESCRIPTION:
 *              this function set ether type by entry,and validate it
 *  INPUT:
 *              devNum - uint8 :  dev num
 *              entryNum - unit32 : the entry who's info be want to set
 *              etherType  - uint16 : ether type we want to set for this entry
 * OUTPUT:
 *
 * RETURN:
 *              PROTOCOL_VLAN_RETURN_CODE_SUCCESS    -  operation success
 *              PROTOCOL_VLAN_RETURN_CODE_NAM_ERR    -   operation failed
 * NOTE:
 *
 *************************************************************/
int nam_prot_vlan_set_entry_eth_type
(
    unsigned char  devNum,
    unsigned int   entryNum,
    unsigned short etherType
);

/*************************************************************
 * DESCRIPTION:
 *              this function disable protocol vlan by entry
 *  INPUT:
 *              devNum - uint8 :  dev num
 *              entryNum - unit32 : the entry who's info be want
 * OUTPUT:
 *
 * RETURN:
 *              PROTOCOL_VLAN_RETURN_CODE_SUCCESS    -  operation success
 *              PROTOCOL_VLAN_RETURN_CODE_NAM_ERR    -   operation failed
 * NOTE:
 *
 *************************************************************/
int nam_prot_vlan_disable_by_entry
(
    unsigned char devNum,
    unsigned int  entryNum
);

/*************************************************************
 * DESCRIPTION:
 *              this function set protocol vlan ID for the very port and entry
 *  INPUT:
 *              devNum - uint8 :  dev num
 *              portNum - uint8 : port num
 *              entryNum - unit32 : the entry who's info be want
 *              vlanId - uint16 : set to this vlanId
 * OUTPUT:
 *
 * RETURN:
 *              PROTOCOL_VLAN_RETURN_CODE_SUCCESS    -  operation success
 *              PROTOCOL_VLAN_RETURN_CODE_NAM_ERR    -   operation failed
 * NOTE:
 *
 *************************************************************/
int nam_prot_vlan_set_vlan_by_port_entry
(
    unsigned char devNum,
    unsigned char portNum,
    unsigned int  entryNum,
    unsigned short  vlanId
);

/*************************************************************
 * DESCRIPTION:
 *              this function get protocol vlan ID and isvalid of the very port and entry
 *  INPUT:
 *              devNum - uint8 :  dev num
 *              portNum - uint8 : port num
 *              entryNum - unit32 : the entry who's info be want
 * OUTPUT:
 *              vlanId  - uint16 : the vlanId we got
 *              isValid  - uint32 : is valid ?
 *                                          TRUE  - is valid
 *                                          FALSE  - is invalid
 *
 * RETURN:
 *              PROTOCOL_VLAN_RETURN_CODE_SUCCESS    -  operation success
 *              PROTOCOL_VLAN_RETURN_CODE_NAM_ERR    -   operation failed
 * NOTE:
 *
 *************************************************************/

int nam_prot_vlan_get_vlan_by_port_entry
(
    unsigned char devNum,
    unsigned char portNum,
    unsigned int  entryNum,
    unsigned short *vlanId,
    unsigned int *isValid
);

/*************************************************************
 * DESCRIPTION:
 *              this function set port protocol vlan to enable/disable
 *  INPUT:
 *              devNum - uint8 :  dev num
 *              portNum - uint8 : port num
 *              isEnable - unint32 : is set to enable or disable
 * OUTPUT:
 *
 * RETURN:
 *              PROTOCOL_VLAN_RETURN_CODE_SUCCESS    -  operation success
 *              PROTOCOL_VLAN_RETURN_CODE_NAM_ERR    -   operation failed
 * NOTE:
 *
 *************************************************************/

int nam_prot_vlan_port_enable_set
(
    unsigned char devNum,
    unsigned char portNum,
    unsigned int  isEnable
);

/*************************************************************
 * DESCRIPTION:
 *              this function invalidate protocol vlan by port and entry
 *  INPUT:
 *              devNum - uint8 :  dev num
 *              portNum - uint8 : port num
 *              entryNum - unit32 : the entry we want to set
 * OUTPUT:
 *
 * RETURN:
 *              PROTOCOL_VLAN_RETURN_CODE_SUCCESS    -  operation success
 *              PROTOCOL_VLAN_RETURN_CODE_NAM_ERR    -   operation failed
 * NOTE:
 *
 *************************************************************/

int nam_prot_vlan_disable_by_port_entry
(
    unsigned char devNum,
    unsigned char portNum,
    unsigned int  entryNum
);

#endif
