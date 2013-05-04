#ifndef __NAM_WIFI_H__
#define __NAM_WIFI_H__

#include <sys/types.h>
#include <sys/socket.h>
#define IN
#define OUT
#define INOUT

/*
 *    GT_FALSE - false.
 *    GT_TRUE  - true.
 */
#define GT_FALSE			0
#define GT_TRUE				1
#define GT_OK              (0x00)   /* Operation succeeded */
#define GT_FAIL            (0x01)   /* Operation failed    */

#define GT_BAD_VALUE       (0x02)   /* Illegal value        */
#define GT_OUT_OF_RANGE    (0x03)   /* Value is out of range*/
#define GT_BAD_PARAM       (0x04)   /* Illegal parameter in function called  */
#define GT_BAD_PTR         (0x05)   /* Illegal pointer value                 */
#define GT_BAD_SIZE        (0x06)   /* Illegal size                          */
#define GT_BAD_STATE       (0x07)   /* Illegal state of state machine        */
#define GT_SET_ERROR       (0x08)   /* Set operation failed                  */
#define GT_GET_ERROR       (0x09)   /* Get operation failed                  */
#define GT_CREATE_ERROR    (0x0A)   /* Fail while creating an item           */
#define GT_NOT_FOUND       (0x0B)   /* Item not found                        */
#define GT_NO_MORE         (0x0C)   /* No more items found                   */
#define GT_NO_SUCH         (0x0D)   /* No such item                          */
#define GT_TIMEOUT         (0x0E)   /* Time Out                              */
#define GT_NO_CHANGE       (0x0F)   /* The parameter is already in this value*/
#define GT_NOT_SUPPORTED   (0x10)   /* This request is not support           */
#define GT_NOT_IMPLEMENTED (0x11)   /* This request is not implemented       */
#define GT_NOT_INITIALIZED (0x12)   /* The item is not initialized           */
#define GT_NO_RESOURCE     (0x13)   /* Resource not available (memory ...)   */
#define GT_FULL            (0x14)   /* Item is full (Queue or table etc...)  */
#define GT_EMPTY           (0x15)   /* Item is empty (Queue or table etc...) */
#define GT_INIT_ERROR      (0x16)   /* Error occured while INIT process      */
#define GT_NOT_READY       (0x1A)   /* The other side is not ready yet       */
#define GT_ALREADY_EXIST   (0x1B)   /* Tried to create existing item         */
#define GT_OUT_OF_CPU_MEM  (0x1C)   /* Cpu memory allocation failed.         */
#define GT_ABORTED         (0x1D)   /* Operation has been aborted.           */


#define BUFF_LEN		5
#define ETH_ALEN 		6
#define MAC_ADDR_LEN		6
#define IP_ADDR_LEN		4
#define NAM_SDMA_TX_PER_BUFFER	1536
#define NAM_PORT_L3INTF_VLAN_ID  4095

extern pthread_mutex_t nexthopHashMutex;
extern pthread_mutex_t arpHashMutex;

#ifdef DRV_LIB_CPSS
extern void * osMemSet
(
    IN void * start,
    IN int    symbol,
    IN unsigned long size
);
#endif
#ifdef DRV_LIB_BCM
#define osMemSet memset
#endif

typedef struct ether_header_t 
{
	unsigned char		dmac[ETH_ALEN];		/* destination eth addr	*/
	unsigned char		smac[ETH_ALEN];		/* source ether addr	*/
	unsigned short		etherType;
}ETHER_HEADER_T;

typedef struct ip_header_t
{
    unsigned char      version:4;
    unsigned char      hdrLength:4;
    unsigned char      dscp:6;
    unsigned char      ecn:2;
    unsigned short     totalLen;
    unsigned short     identifier;
    unsigned short     flag:3;
    unsigned short     fragOffset:13;
    unsigned char      ttl;
    unsigned char      ipProtocol;
    unsigned short     checkSum;
    unsigned char      sip[IP_ADDR_LEN];
    unsigned char      dip[IP_ADDR_LEN];
}IP_HEADER_T;


struct vlan_ports_memebers {
	unsigned int		count;
	unsigned int 	ports[128];
};

/* ethenet ARP packet*/
struct arp_packet_t {
	unsigned short 	hwType;				/*hardware type: 0x0001-ethernet*/
	unsigned short 	protocol;			/* protocol type:0x0800-IP*/
	unsigned char 	hwSize;				/* hardware size*/
	unsigned char 	protSize;			/*protocol size*/
	unsigned short 	opCode;				/* 0x0001-request 0x0002-reply*/
	unsigned char  	smac[MAC_ADDR_LEN];	/* sender's MAC address*/
	unsigned char 	sip[IP_ADDR_LEN]; 	/* sender's ip address*/
	unsigned char 	dmac[MAC_ADDR_LEN];	/* target's MAC address*/
	unsigned char 	dip[IP_ADDR_LEN];	/* target's ip address*/
};

/*communicate with WIFI*/
struct wifi_struct
{
	unsigned int len_addr;
	unsigned int data_addr;
};


/**************************************
 *nam_wifi_net_if_rx_packet
 *
 *Transmit packets to wifi
 *
 *
 ***************************************/
long nam_wifi_net_if_rx_packet
( 
    IN unsigned long      	numOfBuff,
    IN unsigned char       	*packetBuffs[],
    IN unsigned long      	buffLen[]
);

#endif
