/*******************************************************************************
 *                Copyright 2004, MARVELL SEMICONDUCTOR, LTD.                   *
 * THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL.                      *
 * NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
 * OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
 * DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
 * THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
 * IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
 *                                                                              *
 * MARVELL COMPRISES MARVELL TECHNOLOGY GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, *
 * MARVELL INTERNATIONAL LTD. (MIL), MARVELL TECHNOLOGY, INC. (MTI), MARVELL    *
 * SEMICONDUCTOR, INC. (MSI), MARVELL ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K.  *
 * (MJKK), MARVELL ISRAEL LTD. (MSIL).                                          *
 ********************************************************************************        
 * genDrvEthPortCtrl.c
 *
 * DESCRIPTION:
 *       This file includes all needed functions for receiving packet buffers
 *       from upper layer, and sending them trough the PP's NIC.
 *
 * DEPENDENCIES:
 *       None.
 *
 * FILE REVISION NUMBER:
 *       $Revision: 1.1.1.1 $
 *
 *******************************************************************************/
#define  _SVID_SOURCE
#include <net/if.h>
#include <net/if_arp.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>                  /* errno        */
#include <string.h>                 /* strerror     */
#include <sys/ioctl.h>              /* ioctl        */
#include <unistd.h>                 /* write        */
#include <sys/select.h>

#ifdef XCAT_DRV
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgVlan.h> 
#endif

#include <gtOs/gtOsGen.h>
#include <gtExtDrv/drivers/gtEthPortCtrl.h>
#include <gtExtDrv/drivers/gtIntDrv.h>
#include "kerneldrv/include/presteraSmiGlob.h"

#ifdef XCAT_DRV
#define MV_HIGH_MEMORY_PREFIX 0xe0000000
#else
#define MV_HIGH_MEMORY_PREFIX 0xd0000000
#endif

static GT_STATUS _osPhy2Virt
(
    IN  GT_U32 phyAddr,
    OUT GT_U32 *virtAddr
)
{
  GT_U32 phyAddr_temp = phyAddr;

  if (phyAddr_temp & MV_HIGH_MEMORY_PREFIX) /* logical address from lsp: mv_high_memory_vaddr*/
    phyAddr_temp &= ~MV_HIGH_MEMORY_PREFIX;
  
  return osPhy2Virt(phyAddr_temp, virtAddr);      
}

/* debug flag for this module, must be commented in release! */
/* #define PRESTERA_SMI_DEBUG */

#ifdef PRESTERA_SMI_DEBUG
# define DBG_INFO(x)     fprintf x
#else
# define DBG_INFO(x)
#endif

extern GT_32 gtPpFd;                /* pp file descriptor           */
#define MAX_SEG     100             /* maximum segments per packet  */

static GT_U32 netIfTid;
static GT_U32 rawSocketTid;
static GT_BOOL rawSocketMode = GT_FALSE;
static extDrvEthTxMode_ENT gTxMode = extDrvEthTxMode_asynch_E;

#ifdef GT_SMI
#ifdef XCAT_DRV
static GT_BOOL txModeSetDone = GT_FALSE;
#endif
#endif

#define SOCK_RAW_MV SOCK_PACKET
#define MAX_DEVICES 256
#define MAXPACKET 2048

static int activeDevice[MAX_DEVICES];
static int sockFD[MAX_DEVICES];
static GT_BOOL openAllNicsDone = GT_FALSE;

void gc_dump ( char *buf, int len )
{
#define isprint(c) ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))

  int offs, i;
  int j;

  for ( offs = 0; offs < len; offs += 16 )
  {
    j = 1;
    osPrintf ( "%08x   ", offs );
    for ( i = 0; i < 16 && offs + i < len; i++ )
    {
      osPrintf ( "%02x", ( unsigned char ) buf[offs + i] );
      if ( ! ( ( j++ ) %4 ) && ( j < 16 ) )
        osPrintf ( "," );
    }
    for ( ; i < 16; i++ )
      osPrintf ( "   " );
    osPrintf ( "  " );
    for ( i = 0; i < 16 && offs + i < len; i++ )
      osPrintf ( "%c",
                   isprint ( buf[offs + i] ) ? buf[offs + i] : '.' );
    osPrintf ( "\n" );
  }
  osPrintf ( "\n" );
}
static GT_Rx_FUNCPTR userRxCbFunc = NULL;
static GT_RawSocketRx_FUNCPTR userRawSocketRxCbFunc = NULL;
static GT_Tx_COMPLETE_FUNCPTR userTxCbFunc = NULL;

static void rem_crlf(char *buf)
{
  char *cp;
  int len = strlen(buf);

  buf[len] = 0;

  if ((cp = strchr(buf, '\n')) != NULL)
    *cp = 0;
  if ((cp = strchr(buf, '\r')) != NULL)
    *cp = 0;
}

static int makeSocket(int device)
{
  int fd;
  struct ifreq ifr;
  struct sockaddr_ll sa;

  fd = socket(PF_PACKET, SOCK_RAW_MV, htons(ETH_P_ALL));

  if (fd < 0) 
  {
    perror("rx socket error");
    exit (1);
  }

  sprintf(ifr.ifr_name, "p%d", device);
    
  /* get the index */
  if (ioctl(fd, SIOCGIFINDEX, &ifr) < 0)
  {
    perror("ioctl: SIOCGIFINDEX");
    exit (1);
  }
    
  /* Bind to the interface name */
  memset(&sa, 0, sizeof(sa));
  sa.sll_family = AF_PACKET;
  sa.sll_ifindex = ifr.ifr_ifindex;
  sa.sll_protocol = htons(ETH_P_ALL);
  sa.sll_hatype = ARPHRD_ETHER;
    
  if (bind(fd, (struct sockaddr *)&sa, sizeof(sa))) 
  {
    perror("bind error");
    exit(1);
  }
    
  /* Base the buffer size on the interface MTU */
  if (ioctl(fd, SIOCGIFMTU, &ifr) < 0 )
  {
    perror("ioctl SIOCGIFMTU");
    exit(1);
  }    
  return fd;
}

static GT_STATUS openAllNics(int *sockFD)
{
  int *fd = sockFD;
  int i, j;
  FILE *fp;
  char buf[128];
  char dev_name[16];
  int num;
  GT_BOOL activeDeviceFound = GT_FALSE;
  
  memset(activeDevice, 0, sizeof(activeDevice));
  fp = popen("/sbin/ifconfig -a", "r");
  while (fgets(buf, sizeof(buf), fp))
  {
    if(strstr(buf, "Link encap:"))
    {
      if (buf[0] != 'p')
        continue;      
      sscanf(&buf[1], "%d", &num);
      sprintf(dev_name, "p%d", num);
      for (j = 0; j < 2; j++) /* try the next 2  lines */
      {
        fgets(buf, sizeof(buf), fp);
        rem_crlf(buf);
        if (strstr(buf, "UP ") && strstr(buf, "RUNNING "))
        {
          activeDeviceFound = GT_TRUE;
          activeDevice[num] = 1;
          continue;
        }
      }
    }
  }
  fclose(fp);    

  if (activeDeviceFound == GT_FALSE)
    return GT_FAIL;
  
  for (i = 0; i < MAX_DEVICES; i++)
  {
    if (activeDevice[i])
      fd[i] = makeSocket(i);
  }
  return GT_OK;
}

static GT_STATUS sendRawSocket
(
    IN GT_U8_PTR        segmentList[],
    IN GT_U32           segmentLen[],
    IN GT_U32           numOfSegments,
    IN GT_U32           txQueue
)
{
  /* this function can be called in synch mode only! */
  int i;
  int len = 0;
  char *packet;
  char *p;
  int dsa_tag, port_num;

  for (i = 0; i < numOfSegments; i++)
    len += segmentLen[i];

  if (len == 0)
    return GT_OK;

  p = packet = (char *)malloc(len);  
  
  for (i = 0; i < numOfSegments; i++)
  {
    osMemCpy(p, segmentList[i], segmentLen[i]);
    p += segmentLen[i];
  }

  dsa_tag = *(int *)&packet[12];
  port_num = (dsa_tag >> 11) & 0x1f;
  if(sendto(sockFD[port_num], packet, len, 0,NULL, 0) < 0)
  {
    osPrintf("sendto error. packet data:\n");
  }
  
  if ((gTxMode == extDrvEthTxMode_synch_E) && (userTxCbFunc != NULL))
    userTxCbFunc(segmentList, numOfSegments);
  
  free(packet);
  return GT_OK;
}


/*******************************************************************************
 * intTask
 *
 * DESCRIPTION:
 *       Network Interface Interrupt handler task.
 *
 * INPUTS:
 *       param1  - device number
 *       param2  - ISR cookie
 *
 * OUTPUTS:
 *       None
 *
 * RETURNS:
 *       GT_FAIL - on error
 *
 * COMMENTS:
 *       None
 *
 *******************************************************************************/
unsigned __TASKCONV netIfintTask(GT_VOID* unused)
{   
  static unsigned long   fifoSeg[MAX_SEG*2+1]; /* FIFO segment array, filled by NetIf  */
  static unsigned char   *segmentListPtr[MAX_SEG];/* segment array used for Rx/TxEnd   */
  static unsigned long   segmentLen[MAX_SEG];  /* segment array length used for Rx     */
  unsigned long   numOfSegments;          /* number of segments in segment array       */
  unsigned long   queueNum;               /* Rx packet queue number                    */
  unsigned long   rc;                     /* return code                               */
  unsigned short  fromRxFifo;
  unsigned long   fifoCtrl;
  unsigned long   fifoIdx;
  unsigned long   i;                      /* iterator                                  */  
  size_t          count; 
    
  DBG_INFO((stderr,"netIfintTask-Start %s\n", strerror(errno)));
  /* No need to wait because not interrupts in the driver until intEnable */
  while (1)
  {
    /* Wait for interrupt */
    while (ioctl (gtPpFd, PRESTERA_SMI_IOC_NETIF_WAIT, NULL) != 0)
    {
      if (errno != EINTR)
      {
        fprintf(stderr, "netIfintTask: Interrupt wait failed: errno(%s)\n",
                strerror(errno));
        return (int)GT_FAIL;
      }
      fprintf(stderr, "netIfintTask: Interrupt wait restart: errno(%s)\n",strerror(errno));
    }
    DBG_INFO((stderr, "netIfintTask:After IOCTL_WAIT: errno(%s)\n",strerror(errno)));

    /* read all Rx packets and Tx Complete segments from the fifo */
    while ((count = read(gtPpFd, fifoSeg, sizeof(fifoSeg))) > 0)
    {
      /* set the number of segments and type of data */
      fifoCtrl = fifoSeg[0];
      numOfSegments = fifoCtrl & ETH_PORT_FIFO_ELEM_CNT_MASK;
      fromRxFifo = (fifoCtrl & ETH_PORT_FIFO_TYPE_RX_MASK) ? 1 : 0;

      if (fromRxFifo)
      {
        DBG_INFO((stderr,"Rx "));

        queueNum = (fifoCtrl & ETH_PORT_FIFO_QUE_NUM_MASK) >>
          ETH_PORT_FIFO_QUE_NUM_OFFSET;

        /* set the segment list and length from fifo data */
        for (fifoIdx = 1, i = 0; i < numOfSegments; i++)
        {
          /* update the segment list for Rx */
          _osPhy2Virt((GT_U32)fifoSeg[fifoIdx++],(GT_U32*)&segmentListPtr[i]);

          DBG_INFO((stderr,"netIfintTask:segmentListPtr[i]=0x%X\n",segmentListPtr[i]));                    

          /* and the segment length for Rx */
          segmentLen[i] = fifoSeg[fifoIdx++];

          DBG_INFO((stderr," seg[%d]= 0x%x (%d);", (int)i,
                    (int)segmentListPtr[i], (int)segmentLen[i]));
#ifdef PRESTERA_SMI_DEBUG
          gc_dump((char *)segmentListPtr[i], segmentLen[i]);
#endif
        }
        if (userRxCbFunc != NULL)
        {
            rc = userRxCbFunc(segmentListPtr, (GT_U32*)segmentLen, numOfSegments, queueNum);
            if (rc != GT_OK)
            {
                fprintf(stderr,"interruptEthPortRxSR err= %d\n", (int)rc);
            }
        }

      }
      else
      {
        DBG_INFO((stderr,"TxEnd "));

        /* set the segment list from fifo data */
        for (fifoIdx = 1, i = 0; i < numOfSegments; i++)
        {
          /* update the segment list for TxEnd */
          _osPhy2Virt((GT_U32)fifoSeg[fifoIdx++],(GT_U32*)&segmentListPtr[i]);
          DBG_INFO((stderr,"netIfintTask:segmentListPtr[i]=0x%X\n",segmentListPtr[i]));

          DBG_INFO((stderr," seg[%d]= 0x%x;", (int)i, (int)segmentListPtr[i]));
        }
        if (userTxCbFunc != NULL)
        {
            rc = userTxCbFunc(segmentListPtr, numOfSegments);
            if (rc != GT_OK)
            {
                fprintf(stderr,"interruptEthPortTxEndSR err= %d\n", (int)rc);
            }
        }
      }
    } /* while (read()) */
    if (count < 0)
    {
      fprintf(stderr, "read error: %s\n", strerror(errno));
    }
         
  }/* while (1)*/
}

/*******************************************************************************
 * rawSocketTask
 *
 * DESCRIPTION:
 *       Network Interface Interrupt handler task.
 *
 * INPUTS:
 *       param1  - device number
 *       param2  - ISR cookie
 *
 * OUTPUTS:
 *       None
 *
 * RETURNS:
 *       GT_FAIL - on error
 *
 * COMMENTS:
 *       None
 *
 *******************************************************************************/
unsigned __TASKCONV rawSocketTask(GT_VOID* unused)
{   
  fd_set rfds;
  int maxfd = 0;
  int clilen;
  int n;
  unsigned long rc;
  int i;
  char *packet;
    
  DBG_INFO((stderr,"rawSocketTask-Start %s\n", strerror(errno)));


  /* open all active nics */
  rc = openAllNics(sockFD);

  openAllNicsDone = GT_TRUE;
  if (rc != GT_OK)
  {
    /* no pnn nics exist. get out */
    return 0;
  }
  
  fprintf(stderr, "rawSocketTask reading raw packets from devices: ");
  
  for (i = 0; i < MAX_DEVICES; i++)
  {
    if (activeDevice[i])
      fprintf(stderr, "p%d ", i);
  }
  fprintf(stderr, "\n");
  
  for ( ; ; )
  {
    FD_ZERO(&rfds);
    for (i = 0; i < MAX_DEVICES; i++)
    {
      if (activeDevice[i])
      {
        FD_SET(sockFD[i], &rfds);
        maxfd = (maxfd < sockFD[i]) ? sockFD[i] : maxfd;
      }
    }

    maxfd++;
    if (select(maxfd, &rfds, NULL, NULL, NULL) < 0) 
    {
      perror("select()");
      exit(1);
    }

    for (i = 0; i < MAX_DEVICES; i++)
      if (FD_ISSET(sockFD[i], &rfds)) 
      {
        clilen = MAXPACKET;
        packet = (char *)osMalloc(MAXPACKET);

        packet +=4; 

        /* printf(">>>> malloced packet=0x%08x\n", packet); */

        n = recvfrom(sockFD[i], packet, MAXPACKET, 0, NULL, (socklen_t *)&clilen);
        if (n < 0)
        {
          perror("read_raw_packets: recvfrom error");
          printf(">>>> packet=0x%08x\n", packet);
          exit(1);
        }
        
        /* all packets arriving here have the full dsa tag.
           "i" is the port number */

        if (userRawSocketRxCbFunc != NULL)
          userRawSocketRxCbFunc((GT_U8_PTR)packet, n, 0);
      }
  }
  /* no return */
}

/*******************************************************************************
 * extDrvNetIfIntConnect
 *
 * DESCRIPTION:
 *       Connect network interface rx/tx events.
 *
 * INPUTS:
 *       None
 *
 * OUTPUTS:
 *       None
 *
 * RETURNS:
 *       GT_OK   - on success
 *       GT_FAIL - on error
 *
 * COMMENTS:
 *       None
 *
 *******************************************************************************/
GT_U32 extDrvNetIfIntConnect
(
 GT_VOID
 )
{
  static GT_U32 intialized = 0;
  char taskName[32];

  if (rawSocketMode == GT_TRUE)
  {
    sprintf(taskName, "rawSocketTask");
    
    osTaskCreate(taskName, 0, 0xa000, rawSocketTask , NULL, &rawSocketTid);
    
    if (0 == rawSocketTid)
    {
      fprintf(stderr, "extDrvNetIfIntConnect: taskSpawn(%s): %s\n", taskName, strerror(errno));
      return GT_FAIL;
    }    
  }

  if (intialized)
  {
    fprintf(stderr, "extDrvNetIfIntConnect: intialized: %s\n", strerror(errno));
    return GT_FAIL;
  }
  intialized = 1;

  sprintf(taskName, "netIfIntTask");
    
  osTaskCreate(taskName, 0, 0xa000, netIfintTask, NULL, &netIfTid);

  if (0 == netIfTid)
  {
    fprintf(stderr, "extDrvNetIfIntConnect: taskSpawn(%s): %s\n", taskName, strerror(errno));
    return GT_FAIL;
  }

  return  GT_OK;
}

/*******************************************************************************
 * extDrvEthPortRxInit
 *
 * DESCRIPTION: Init the ethernet port Rx interface
 *
 * INPUTS:
 *       rxBufPoolSize   - buffer pool size
 *       rxBufPool_PTR   - the address of the pool
 *       rxQNum          - the number of RX queues
 *       rxQbufPercentage- the buffer percentage dispersal for all queues
 *       rxBufSize       - the buffer requested size
 *       numOfRxBufs_PTR - number of requested buffers, and actual buffers created
 *       headerOffset    - packet header offset size
 *
 * OUTPUTS:
 *       None.
 *
 * RETURNS:
 *       GT_OK if successful, or
 *       GT_FAIL otherwise.
 *
 * COMMENTS:
 *       None.
 *
 *******************************************************************************/
GT_STATUS extDrvEthPortRxInit
(
 IN GT_U32           rxBufPoolSize,
 IN GT_U8_PTR        rxBufPool_PTR,
 IN GT_U32           rxQNum,
 IN GT_U32           rxQbufPercentage[],
 IN GT_U32           rxBufSize,
 INOUT GT_U32        *numOfRxBufs_PTR,
 IN GT_U32           headerOffset
 )
{

  RX_INIT_PARAM       initParam;
  GT_U32        i;


  osVirt2Phy((GT_U32)rxBufPool_PTR, (GT_U32*)(&initParam.rxBufPoolPtr));
  initParam.rxBufPoolSize     = rxBufPoolSize;          
  initParam.rxBufSize         = rxBufSize;
  initParam.numOfRxBufsPtr    = (unsigned long *)numOfRxBufs_PTR;
  initParam.headerOffset      = headerOffset;
  initParam.rxQNum            = rxQNum = 1; /* LSP support 4 RX Queues */

  for(i = 0; i < GT_MAX_RX_QUEUE_CNS;i++)
  {
    initParam.rxQbufPercentage[i] = rxQbufPercentage[i];
  }
    
  
  if (ioctl(gtPpFd, PRESTERA_SMI_IOC_ETHPORTRXINIT, &initParam) != 0)
  {
    fprintf(stderr, "Fail to init ethernet port Rx, errno(%s)\n",
            strerror(errno));
    return GT_FAIL;
  }

  extDrvNetIfIntConnect();

  return GT_OK;
}

/*******************************************************************************
 * extDrvEthRawSocketModeSet
 *
 * DESCRIPTION: Sets the raw packet mode
 *
 * INPUTS:
 *       flag      - GT_TRUE  - rawSocketMode is set to GT_TRUE
 *                 - GT_FALSE - rawSocketMode is set to GT_FALSE
 *
 * OUTPUTS:
 *       None.
 *
 * RETURNS:
 *       GT_OK always
 *
 * COMMENTS:
 *       None.
 *
 *******************************************************************************/
GT_STATUS extDrvEthRawSocketModeSet
(
 IN GT_BOOL          flag
 )
{
  rawSocketMode = flag;
  return GT_OK;
}

/*******************************************************************************
* extDrvLinuxModeSet
*
* DESCRIPTION:
*       Set port <portNum> to Linux Mode (Linux Only)
*
* INPUTS:
*       portNum         - The port number to be defined for Linux mode
*       ip1, ip2, ip3, ip4 - The ip address to assign to the port, 4 numbers
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - always,
*
* COMMENTS:
*       None.
*
*******************************************************************************/

GT_STATUS extDrvLinuxModeSet
(
 IN GT_U32 portNum,
 IN GT_U32 ip1,
 IN GT_U32 ip2,
 IN GT_U32 ip3,
 IN GT_U32 ip4
)
{
#ifdef XCAT_DRV
  char buf[128];
  GT_U8   dev;
  GT_U16 vid;
  CPSS_PORTS_BMP_STC  portsMembers;
  CPSS_PORTS_BMP_STC  portsTagging;
  CPSS_DXCH_BRG_VLAN_INFO_STC  cpssVlanInfo;
  CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC portsTaggingCmd;
  
  sprintf(buf, "ifconfig p%d up %d.%d.%d.%d", portNum, ip1, ip2, ip3, ip4);
  extDrvEthMuxSet(portNum, extDrvEthNetPortType_linux_E);

  system(buf);

  /* write vlan entry 4000 to direct <portNum> packets to cpu */
  
  dev = 0;
  vid = 4000;
  osMemSet(&portsMembers, 0, sizeof(CPSS_PORTS_BMP_STC));
  osMemSet(&portsTagging, 0, sizeof(CPSS_PORTS_BMP_STC));
  osMemSet(&portsTaggingCmd, 0, sizeof(CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC));
  osMemSet(&cpssVlanInfo, 0, sizeof(cpssVlanInfo));

  CPSS_PORTS_BMP_PORT_SET_MAC(&portsMembers, portNum);
  portsTaggingCmd.portsCmd[portNum] = CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E;  
  
  cpssVlanInfo.unkSrcAddrSecBreach = GT_FALSE; 
  cpssVlanInfo.unregNonIpMcastCmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E; 
  cpssVlanInfo.unregIpv4McastCmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E; 
  cpssVlanInfo.unregIpv6McastCmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E; 
  cpssVlanInfo.unkUcastCmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E; 
  cpssVlanInfo.unregIpv4BcastCmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E; 
  cpssVlanInfo.unregNonIpv4BcastCmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E; 
  cpssVlanInfo.ipv4IgmpToCpuEn = GT_FALSE; 
  cpssVlanInfo.mirrToRxAnalyzerEn = GT_FALSE; 
  cpssVlanInfo.ipv6IcmpToCpuEn = GT_FALSE; 
  cpssVlanInfo.ipCtrlToCpuEn = CPSS_DXCH_BRG_IP_CTRL_NONE_E; 
  cpssVlanInfo.ipv4IpmBrgMode = CPSS_BRG_IPM_SGV_E; 
  cpssVlanInfo.ipv6IpmBrgMode = CPSS_BRG_IPM_SGV_E; 
  cpssVlanInfo.ipv4IpmBrgEn = GT_FALSE; 
  cpssVlanInfo.ipv6IpmBrgEn = GT_FALSE; 
  cpssVlanInfo.ipv6SiteIdMode = CPSS_IP_SITE_ID_INTERNAL_E; 
  cpssVlanInfo.ipv4UcastRouteEn = GT_FALSE; 
  cpssVlanInfo.ipv4McastRouteEn = GT_FALSE; 
  cpssVlanInfo.ipv6UcastRouteEn = GT_FALSE; 
  cpssVlanInfo.ipv6McastRouteEn = GT_FALSE; 
  cpssVlanInfo.stgId = 0; 
  cpssVlanInfo.autoLearnDisable = GT_TRUE; 
  cpssVlanInfo.naMsgToCpuEn = GT_FALSE; 
  cpssVlanInfo.mruIdx = 0; 
  cpssVlanInfo.bcastUdpTrapMirrEn = GT_FALSE; 
  cpssVlanInfo.vrfId = 0; 
  cpssVlanInfo.floodVidx = 4095; 
  cpssVlanInfo.floodVidxMode = CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_UNREG_MC_E; 
  cpssVlanInfo.portIsolationMode = CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_DISABLE_CMD_E; 
  cpssVlanInfo.ucastLocalSwitchingEn = GT_TRUE; 
  cpssVlanInfo.mcastLocalSwitchingEn = GT_TRUE;  
  
  cpssDxChBrgVlanEntryWrite(dev, 
                            vid,
                            &portsMembers,
                            &portsTagging,
                            &cpssVlanInfo,
                            &portsTaggingCmd);  


  /* associate <portNum> with vlanid 4000 */

  cpssDxChBrgVlanPortVidSet(dev, portNum, vid);

  sprintf(buf, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);
  printf("\nextDrvLinuxModeSet, setting port %d mode=linux, ip=%s, vlanid=%d\n\n",
         portNum, buf, vid);
#endif
  return GT_OK;
}              

/*******************************************************************************
* extDrvLinuxModeGet
*
* DESCRIPTION:
*       Get port <portNum> Linux Mode indication (Linux Only)
*
* INPUTS:
* INPUTS:
*       portNum         - The port number to be defined for Linux mode
* OUTPUTS:
*
* RETURNS:
*       GT_TRUE if Linux mode, or
*       GT_FALSE otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/

GT_BOOL extDrvLinuxModeGet
(
 IN GT_U32  portNum
)
{
  return GT_FALSE;
}              

/*******************************************************************************
 * extDrvEthRawSocketModeGet
 *
 * DESCRIPTION: returns the raw packet mode
 *
 * INPUTS:
 *       None:
 *
 * OUTPUTS:
 *       None.
 *
 * RETURNS:
 *       GT_BOOL indicating raw mode is true or false.
 *
 * COMMENTS:
 *       None.
 *
 *******************************************************************************/
GT_BOOL extDrvEthRawSocketModeGet
(
 IN GT_VOID
 )
{
  return rawSocketMode;
}

/*******************************************************************************
 * extDrvEthMuxSet
 *
 * DESCRIPTION: Sets the mux mode to one of cpss, raw, linux
 *
 * INPUTS:
 *       portNum   - The port number for the action
 *       portType  - one of the modes: cpss, raw, Linux
 *
 * OUTPUTS:
 *       None.
 *
 * RETURNS:
 *       GT_OK always
 *
 * COMMENTS:
 *       None.
 *
 *******************************************************************************/
GT_STATUS extDrvEthMuxSet
(
 IN unsigned long portNum,
 IN extDrvEthNetPortType_ENT portType
 )
{
  MUX_PARAM muxParam;
  
  muxParam.portNum = portNum;
  muxParam.portType = portType;
  
  if (ioctl(gtPpFd, PRESTERA_SMI_IOC_MUXSET, &muxParam) != 0)
  {
    fprintf(stderr, "Fail to set port %d mux to port-type %d, errno(%s)\n",
            portNum, (int)portType, strerror(errno));
    return GT_FAIL;
  }
    
  return GT_OK;
}

/*******************************************************************************
 * extDrvEthMuxGet
 *
 * DESCRIPTION: Get the mux mosde of the port
 *
 * INPUTS:
 *       portNum   - The port number for the action
 *
 * OUTPUTS:
 *       portType  - port type information
 *
 * RETURNS:
 *       GT_OK always
 *
 * COMMENTS:
 *       None.
 *
 *******************************************************************************/
GT_STATUS extDrvEthMuxGet
(
 IN unsigned long portNum,
 OUT extDrvEthNetPortType_ENT *portTypeP
 )
{
  MUX_PARAM muxParam;
  
  muxParam.portNum = portNum;
  
  if (ioctl(gtPpFd, PRESTERA_SMI_IOC_MUXGET, &muxParam) != 0)
  {
    fprintf(stderr, "Fail to get mux info for port %d, errno(%s)\n",
            portNum, strerror(errno));
    return GT_FAIL;
  }

  *portTypeP = muxParam.portType;    
  return GT_OK;
}

/*******************************************************************************
 * extDrvEthPortRxInitWithoutInterruptConnection
 *
 * DESCRIPTION: this function initialises  ethernet port Rx interface without interrupt 
 *              connection
 *              The function used in multiprocess appDemo for eth port init in appDemo 
 *              process. In RxTxProcess used extDrvEthPortRxInit that run 
 *              RxTxInterrupt thread 
 *              
 *
 * INPUTS:
 *  rxBufPoolSize - buffer pool size
 *  rxBufPool_PTR - the address of the pool
 *  rxQNum      - the number of RX queues
 *  rxQbufPercentage  - the buffer percentage dispersal for all queues
 *  rxBufSize - the buffer requested size
 *  numOfRxBufs_PTR - number of requested buffers, and actual buffers created
 *  headerOffset  - packet header offset size
 *
 * OUTPUTS:
 *       None.
 *
 * RETURNS:
 *       GT_OK if successful, or
 *       GT_FAIL otherwise.
 *
 * COMMENTS:
 *       None.
 *
 *******************************************************************************/
GT_STATUS extDrvEthPortRxInitWithoutInterruptConnection
(
 IN GT_U32           rxBufPoolSize,
 IN GT_U8_PTR        rxBufPool_PTR,
 IN GT_U32           rxQNum,
 IN GT_U32           rxQbufPercentage[],
 IN GT_U32           rxBufSize,
 INOUT GT_U32        *numOfRxBufs_PTR,
 IN GT_U32           headerOffset
 )
{

  RX_INIT_PARAM       initParam;
  GT_U32        i;

   
  osVirt2Phy((GT_U32)rxBufPool_PTR, (GT_U32*)(&initParam.rxBufPoolPtr));

  initParam.rxBufPoolSize     = rxBufPoolSize;          
  initParam.rxBufSize         = rxBufSize;
  initParam.numOfRxBufsPtr    = (unsigned long *)numOfRxBufs_PTR;
  initParam.headerOffset      = headerOffset;
  initParam.rxQNum            = rxQNum = 4; /* LSP support 4 RX Queues */

  for(i = 0; i < GT_MAX_RX_QUEUE_CNS;i++)
  {
    initParam.rxQbufPercentage[i] = rxQbufPercentage[i];
  }
    
  
  if (ioctl(gtPpFd, PRESTERA_SMI_IOC_ETHPORTRXINIT, &initParam) != 0)
  {
    fprintf(stderr, "Fail to init ethernet port Rx, errno(%s)\n",
            strerror(errno));
    return GT_FAIL;
  }

  /* 
     extDrvNetIfIntConnect();
  */

  return GT_OK;
}


/*******************************************************************************
 * extDrvEthPortTxInit
 *
 * DESCRIPTION: Init the ethernet port Tx interface
 *
 * INPUTS:
 *       numOfTxBufs - number of requested buffers
 *
 * OUTPUTS:
 *       None.
 *
 * RETURNS:
 *       GT_OK if successful, or
 *       GT_FAIL otherwise.
 *
 * COMMENTS:
 *       None.
 *
 *******************************************************************************/
GT_STATUS extDrvEthPortTxInit
(
 IN GT_U32           numOfTxBufs
 )
{
  
  if (ioctl(gtPpFd, PRESTERA_SMI_IOC_ETHPORTTXINIT, &numOfTxBufs) != 0)
  {
    fprintf(stderr, "Fail to init ethernet port Tx, errno(%s)\n",
            strerror(errno));
    return GT_FAIL;
  }
  
  return GT_OK;
}


/*******************************************************************************
 * extDrvEthPortEnable
 *
 * DESCRIPTION: Enable the ethernet port interface
 *
 * INPUTS:
 *       None
 *
 * OUTPUTS:
 *       None.
 *
 * RETURNS:
 *       GT_OK if successful, or
 *       GT_FAIL otherwise.
 *
 * COMMENTS:
 *       None.
 *
 *******************************************************************************/
GT_STATUS extDrvEthPortEnable
(
 GT_VOID
 )
{
  MUX_PARAM muxParam;
  int i;
  
  if (ioctl(gtPpFd, PRESTERA_SMI_IOC_ETHPORTENABLE, NULL) != 0)
  {
    fprintf(stderr, "Fail to enable ethernet port, errno(%s)\n",
            strerror(errno));
    return GT_FAIL;
  } 
 
  if (rawSocketMode == GT_TRUE)
  {
    /* wait for openAllNics to finish */    
    while(openAllNicsDone == GT_FALSE);

    for (i = 0; i < MAX_DEVICES; i++)
    {
      if (!activeDevice[i])
        continue;
      
      muxParam.portNum = i;
      muxParam.portType = bspEthNetPortType_raw_E;
      
      if (ioctl(gtPpFd, PRESTERA_SMI_IOC_MUXSET, &muxParam) != 0)
      {
        fprintf(stderr, "Fail to set port %d mux to raw, errno(%s)\n",
                i, strerror(errno));
        return GT_FAIL;
      }
    }
  }
  
  return GT_OK;
}


/*******************************************************************************
 * extDrvEthPortDisable
 *
 * DESCRIPTION: Disable the ethernet port interface
 *
 * INPUTS:
 *       None
 *
 * OUTPUTS:fprintf(stderr, "extDrvEthPortEnable:start, errno(%s)\n",strerror(errno));
 *       None.
 *
 * RETURNS:
 *       GT_OK if successful, or
 *       GT_FAIL otherwise.
 *
 * COMMENTS:
 *       None.
 *
 *******************************************************************************/
GT_STATUS extDrvEthPortDisable
(
 GT_VOID
 )
{
  
  if (ioctl(gtPpFd, PRESTERA_SMI_IOC_ETHPORTDISABLE, NULL) != 0)
  {
    fprintf(stderr, "Fail to disable ethernet port, errno(%s)\n",
            strerror(errno));
    return GT_FAIL;
  } 

  return GT_OK;
}


/*******************************************************************************
 * extDrvEthPortRxCb
 *
 * DESCRIPTION:
 *       This function receives a packet from the ethernet port.
 *
 * INPUTS:
 *       segmentList     - A list of pointers to the packets segments.
 *       segmentLen      - A list of segement length.
 *       numOfSegments   - The number of segment in segment list.
 *       queueNum        - the received queue number
 *
 * OUTPUTS:
 *       None.
 *
 * RETURNS:
 *       GT_TRUE if successful, or
 *       GT_FALSE otherwise.
 *
 * COMMENTS:
 *       None.
 *
 *******************************************************************************/
GT_STATUS extDrvEthPortRxCb
(
 IN GT_U8_PTR        segmentList[],
 IN GT_U32           segmentLen[],
 IN GT_U32           numOfSegments,
 IN GT_U32           queueNum
 )
{
  /* this routine is NOT used as ISR for Linux */
  fprintf(stderr, "extDrvEthPortRxCb, errno(%s)\n",strerror(errno));
  return GT_FAIL;
}


/******************************************************************************
 * extDrvEthPortTxEndCb
 *
 * DESCRIPTION:
 *       This function receives a packet from the ethernet port.
 *
 * INPUTS:
 *       segmentList     - A list of pointers to the packets segments.
 *       numOfSegments   - The number of segment in segment list.
 *
 * OUTPUTS:
 *       None.
 *
 * RETURNS:
 *       GT_OK if successful, or
 *       GT_FAIL otherwise.
 *
 * COMMENTS:
 *       None.
 *
 *******************************************************************************/
GT_STATUS extDrvEthPortTxEndCb
(
 IN GT_U8_PTR        segmentList[],
 IN GT_U32           numOfSegments
 )
{
  /* this routine is NOT used as ISR for Linux */
  fprintf(stderr, "extDrvEthPortTxEndCb, errno(%s)\n",strerror(errno));
  return GT_FAIL;
}


/*******************************************************************************
* extDrvEthPortTx
*
* DESCRIPTION:
*       This function transmits a packet through the ethernet port in salsa.
*
* INPUTS:
*       segmentList     - A list of pointers to the packets segments.
*       segmentLen      - A list of segement length.
*       numOfSegments   - The number of segment in segment list.
*       txQueue         - The TX queue.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvEthPortTx
(
    IN GT_U8_PTR        segmentList[],
    IN GT_U32           segmentLen[],
    IN GT_U32           numOfSegments,
    IN GT_U32           txQueue
)
{
  
  GT_U32       segArr[MAX_SEG*2];
  GT_U32       segLenOffset;
  GT_U32       i;
  
  if (numOfSegments > MAX_SEG)
    return GT_FAIL;

  if (rawSocketMode == GT_TRUE)
    return sendRawSocket(segmentList, segmentLen, numOfSegments, txQueue);
  
  /* copy the segments and lengths to the segArr */
  for (segLenOffset = numOfSegments, i = 0 ;
       i < numOfSegments; i++, segLenOffset++)
  {      
    osVirt2Phy((GT_U32)segmentList[i],(GT_U32*)&segArr[i]);   
    segArr[segLenOffset] = segmentLen[i];
    
    /*
      We need a way to trasfer the txQueue number through the "write" system 
      call to "presteraSmi_write" function. 
      Since we cannot add another parameter to the "write" system call, 
      and we don't want to waste ioctl, we embed the txQueue in the 8 
      leftmost bits of segmentLen[0]. segmentLen[0] is always very
      small and will never get to 2^24 size. gc
    */
    
    if (i == 0)
      segArr[segLenOffset] |= (txQueue << 24);
  }
  
  /* and send the packet */
  if (write(gtPpFd, segArr, numOfSegments) > 0)
    return GT_OK;
  else
  {    
    fprintf(stderr, "extDrvEthPortTx:FAIL to send packet, errno(%s)\n",strerror(errno));     
    return GT_FAIL;
  }
  
  return GT_OK;
}

/*******************************************************************************
* extDrvEthPortTxModeSet
*
* DESCRIPTION: Set the ethernet port tx mode
*
* INPUTS:
*       txMode - extDrvEthTxMode_asynch_E - don't wait for TX done - free packet
*                when interrupt received
*                extDrvEthTxMode_asynch_E - wait to TX done and free packet
*                immediately
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise
*       GT_NOT_SUPPORTED
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvEthPortTxModeSet
(
    extDrvEthTxMode_ENT    txMode
)
{
#ifdef GT_SMI
#ifdef XCAT_DRV
  unsigned long txMode_temp = (unsigned long)txMode;

  gTxMode = txMode;

  if (rawSocketMode == GT_TRUE)
  {
    if (txModeSetDone == GT_TRUE)
      return GT_OK; /* allow just one call to lsp in raw sockets */

    txModeSetDone = GT_TRUE;    
  }

  if (ioctl(gtPpFd, PRESTERA_SMI_IOC_TXMODE_SET, &txMode_temp) != 0)
  {
    fprintf(stderr, "Fail to set txMode, errno(%s)\n",
            strerror(errno));
    return GT_FAIL;
  }  
  return GT_OK;

#endif 
    return (txMode == extDrvEthTxMode_asynch_E) ? GT_OK : GT_NOT_SUPPORTED;
#else
    return GT_NOT_SUPPORTED;
#endif
}

/*******************************************************************************
 * extDrvEthInputHookAdd
 *
 * DESCRIPTION:
 *       This bind the user Rx callback
 *
 * INPUTS:
 *       userRxFunc - the user Rx callbak function
 *
 * OUTPUTS:
 *       None.
 *
 * RETURNS:
 *       GT_OK if successful, or
 *       GT_FAIL otherwise.
 *
 * COMMENTS:
 *       None.
 *
 *******************************************************************************/
GT_STATUS extDrvEthInputHookAdd
(
 IN GT_Rx_FUNCPTR    userRxFunc
 )
{
  if (userRxFunc == NULL)
  {
      return GT_FAIL;
  }
  userRxCbFunc = userRxFunc;
  if (ioctl(gtPpFd, PRESTERA_SMI_IOC_ETHPORTRXBIND, NULL) != 0)
  {
    fprintf(stderr, "Fail to bind ethernet port Rx callback, errno(%s)\n",
            strerror(errno));
    return GT_FAIL;
  }
  
  return GT_OK;
}

/*******************************************************************************
 * extDrvEthRawSocketRxHookAdd
 *
 * DESCRIPTION:
 *       bind the raw packet Rx callback
 *
 * INPUTS:
 *       userRawRxFunc - the user ra packet Rx callbak function
 *
 * OUTPUTS:
 *       None.
 *
 * RETURNS:
 *       GT_OK if successful, or
 *       GT_FAIL otherwise.
 *
 * COMMENTS:
 *       None.
 *
 *******************************************************************************/
GT_STATUS extDrvEthRawSocketRxHookAdd
(
 IN GT_RawSocketRx_FUNCPTR    userRxFunc
 )
{
  if (userRxFunc == NULL)
  {
      return GT_FAIL;
  }
  userRawSocketRxCbFunc = userRxFunc;
  return GT_OK;
}


/*******************************************************************************
 * extDrvEthTxCompleteHookAdd
 *
 * DESCRIPTION:
 *       This bind the user Tx complete callback
 *
 * INPUTS:
 *       userTxFunc - the user Tx callback function
 *
 * OUTPUTS:
 *       None.
 *
 * RETURNS:
 *       GT_OK if successful, or
 *       GT_FAIL otherwise.
 *
 * COMMENTS:
 *       None.
 *
 *******************************************************************************/
GT_STATUS extDrvEthTxCompleteHookAdd
(
 IN GT_Tx_COMPLETE_FUNCPTR userTxFunc
 )
{
  if (userTxFunc == NULL)
  {
      return GT_FAIL;
  }
  userTxCbFunc = userTxFunc;

  if (ioctl(gtPpFd, PRESTERA_SMI_IOC_ETHPORTTXBIND, NULL) != 0)
  {
    fprintf(stderr, "Fail to bind ethernet port Tx callback, errno(%s)\n",
            strerror(errno));
    return GT_FAIL;
  } 

  return GT_OK;
}


/*******************************************************************************
 * extDrvEthRxPacketFree
 *
 * DESCRIPTION:
 *       This routine frees the recievd Rx buffer.
 *
 * INPUTS:
 *       segmentList     - A list of pointers to the packets segments.
 *       numOfSegments   - The number of segment in segment list.
 *       queueNum        - receive queue number
 *
 * OUTPUTS:
 *       None.
 *
 * RETURNS:
 *       GT_OK if successful, or
 *       GT_FAIL otherwise.
 *
 * COMMENTS:
 *       None.
 *
 *******************************************************************************/
GT_STATUS extDrvEthRxPacketFree
(
 IN GT_U8_PTR        segmentList[],
 IN GT_U32           numOfSegments,
 IN GT_U32           queueNum
 )
{

  GT_U32   i;
  RX_FREE_BUF_PARAM   buffFreeParam;

  if ( ((GT_U32)segmentList[0] < 0x50000000) ||   
    ((GT_U32)segmentList[0] > 0x70000000) )
  {
    /* it's a malloc'ed raw packet */
    osFree(segmentList[0] - 4);  /* back to osMalloc address */
    return GT_OK;
  }
    
  for(i = 0; i < numOfSegments; i++)
  {
    osVirt2Phy((GT_U32)segmentList[i], (GT_U32 *)&buffFreeParam.segmentList[i]);
    /*
    DBG_INFO((stderr, "extDrvEthRxPacketFree:buffFreeParam.segmentListPtr[i]=%p\n",
              buffFreeParam.segmentList[i]));
    */
  }    
  buffFreeParam.numOfSegments  = numOfSegments;
  buffFreeParam.queueNum       = queueNum;
  
  if (ioctl(gtPpFd, PRESTERA_SMI_IOC_ETHPORTFREEBUF, &buffFreeParam) != 0)
  {
    fprintf(stderr, "Fail to free ethernet port buff, errno(%s)\n",
            strerror(errno));
    return GT_FAIL;
  }
  
  return GT_OK;
}

/*******************************************************************************
* extDrvEthCpuCodeToQueue
*
* DESCRIPTION:
*       Binds DSA CPU code to RX queue.
*
* INPUTS:
*       dsaCpuCode - DSA CPU code
*       rxQueue    -  rx queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on fail
*       GT_NOT_SUPPORTED    - the API is not supported
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvEthCpuCodeToQueue
(
    IN GT_U32  dsaCpuCode,
    IN GT_U8   rxQueue
)
{
  CPU_CODE_TO_QUEUE_PARAM cpuCodeToQueueParam;

  cpuCodeToQueueParam.cpuCode = dsaCpuCode;
  cpuCodeToQueueParam.queue   = rxQueue;

  if (ioctl(gtPpFd, PRESTERA_SMI_IOC_CPUCODE_TO_QUEUE, &cpuCodeToQueueParam) != 0)
  {
    fprintf(stderr, "Fail to set CpuCode to Queue, errno(%s)\n",
            strerror(errno));
    return GT_FAIL;
  }

  return GT_OK;
}

/*******************************************************************************
* extDrvEthPrePendTwoBytesHeaderSet
*
* DESCRIPTION:
*       Enables/Disable pre-pending a two-byte header to all packets arriving
*       to the CPU.
*
* INPUTS:
*        enable    - GT_TRUE  - Two-byte header is pre-pended to packets
*                               arriving to the CPU.
*                    GT_FALSE - Two-byte header is not pre-pended to packets
*                               arriving to the CPU.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on fail
*       GT_NOT_SUPPORTED    - the API is not supported
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvEthPrePendTwoBytesHeaderSet
(
    IN GT_BOOL enable
)
{
    /*CPSS_TBD_BOOKMARK*/
    /* add support */
    return GT_OK;
}
