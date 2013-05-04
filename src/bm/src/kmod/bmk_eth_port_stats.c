/**
  * Ioctl cmds for read board eth port statistics.
  * author: Autelan. Tech.
  * codeby: baolc
  * 2008-07-08
  */
#include <linux/mm.h>
#include <net/dst.h>
#include <asm/octeon/cvmx.h>
#include <asm/octeon/cvmx-pip-defs.h>
#include <asm/octeon/cvmx-gmxx-defs.h>

#include "bmk_main.h"
#include "bmk_eth_port_stats.h"

uint64_t    base_addr64 = (1ull << 63);
#define CPLD1_BASE_ADDR 	0x1d010000
#define CPLD_PHY_LED_CTL 	0x27
#define CPLD_PRODUCT_CTL 	0x1
#define CPLD_CARD_TYPE(x) 	(0x24+x)
#define CPLD_CARD_ON 		0x10
#define AX51_XFP_TYPE 		0x1
#define AX51_GTX_TYPE 		0x2
#define AX51_SFP_TYPE		0x3


static const char *eth_name[] = {
"eth0-1","eth0-2","eth0-3","eth0-4"
};
/* wangchao : remove for warnning
static const char *eth_name_5612i[] = {
"eth1-9","eth1-10","eth1-11","eth1-12"
};*/

/**
  * Read board eth port's statistics.
  * @param port_num: port number.
  * @param clear: if clear=1, clear all statistics after reading CSRs.
  * @param port_counter: all statistics data.
  * @return: 0 for success. -1 for failure.
  */
int do_read_board_eth_port_stats
(
	int port_num, 
	int clear, 
	struct npd_port_counter* port_counter,
	struct port_oct_s* portOctCounter
)
{
	cvmx_pip_stat_ctl_t pip_stat_ctl; /* for clear PIP stats CSRs */
	/* GMX rx stats data */ 
	cvmx_gmxx_rxx_stats_octs_t  gmx_rx_stat_goodbytes;
    cvmx_gmxx_rxx_stats_pkts_ctl_t  gmx_rx_stat_fc_pkts;
	cvmx_gmxx_rxx_stats_pkts_drp_t  gmx_rx_stat_overrun_pkts;

	/* GMX tx stats data */
	cvmx_gmxx_txx_stat0_t  gmx_tx_stat0;
	cvmx_gmxx_txx_stat1_t  gmx_tx_stat1;
	cvmx_gmxx_txx_stat2_t  gmx_tx_stat2;
	cvmx_gmxx_txx_stat3_t  gmx_tx_stat3;
	/*cvmx_gmxx_txx_stat4_t  gmx_tx_stat4;
	cvmx_gmxx_txx_stat5_t  gmx_tx_stat5;
	cvmx_gmxx_txx_stat6_t  gmx_tx_stat6;
	cvmx_gmxx_txx_stat7_t  gmx_tx_stat7;*/
	cvmx_gmxx_txx_stat8_t  gmx_tx_stat8;
	cvmx_gmxx_txx_stat9_t  gmx_tx_stat9;

	struct net_device *dev;
	struct net_device_stats *status;

	
	if (port_num < 0 || port_num > 3 || port_counter == NULL || portOctCounter == NULL)
		return -1;

/* caojia@autelan.com  wipe off for new sdk2.2 update , not support 5612i*/
#if 0
	if(7 == cvm_oct_get_product_type())
	{
		/*the product is 5612i*/
		if (product_is_AU5612i())
		{
			dev = dev_get_by_name(eth_name_5612i[port_num]);
			status = dev->get_stats(dev);
		}
		/*the product is 7k*/
		else
		{
			dev = dev_get_by_name(eth_name[port_num]);
			status = dev->get_stats(dev);
		}
#else
	if (1)
	{
#endif
		dev = dev_get_by_name(&init_net, eth_name[port_num]);

    	/* code optmize: wangchao@autelan.com 2013-01-17 */
        if (dev == NULL) {
            printk("dev_get_by_name Error\n");
			return -1;
		}
		
		status = dev->netdev_ops->ndo_get_stats(dev);
		
		pip_stat_ctl.u64 = 0;
    	pip_stat_ctl.s.rdclr = 0;
		cvmx_write_csr(CVMX_GMXX_RXX_STATS_CTL(port_num, 1), pip_stat_ctl.u64); /* ctl gmx rx stat */
		cvmx_write_csr(CVMX_GMXX_TXX_STATS_CTL(port_num, 1), pip_stat_ctl.u64); /* ctl gmx tx stat */
		
	    /* read gmx rx stats data */
		gmx_rx_stat_goodbytes.u64    = cvmx_read_csr(CVMX_GMXX_RXX_STATS_OCTS(port_num, 1));
		gmx_rx_stat_fc_pkts.u64      = cvmx_read_csr(CVMX_GMXX_RXX_STATS_PKTS_CTL(port_num, 1));
		gmx_rx_stat_overrun_pkts.u64 = cvmx_read_csr(CVMX_GMXX_RXX_STATS_PKTS_DRP(port_num, 1));
		#if 0
		cvmx_read_csr(CVMX_GMXX_RXX_STATS_OCTS_CTL(port_num, 1)); /* read these CSRs just for clear them */
		cvmx_read_csr(CVMX_GMXX_RXX_STATS_OCTS_DMAC(port_num, 1)); /* read these CSRs just for clear them */
		cvmx_read_csr(CVMX_GMXX_RXX_STATS_OCTS_DRP(port_num, 1)); /* read these CSRs just for clear them */
		cvmx_read_csr(CVMX_GMXX_RXX_STATS_PKTS(port_num, 1)); /* read these CSRs just for clear them */
		cvmx_read_csr(CVMX_GMXX_RXX_STATS_PKTS_BAD(port_num, 1)); /* read these CSRs just for clear them */
		cvmx_read_csr(CVMX_GMXX_RXX_STATS_PKTS_DMAC(port_num, 1)); /* read these CSRs just for clear them */
		#endif

		/* read gmx tx stats data */
	    gmx_tx_stat0.u64 = cvmx_read_csr(CVMX_GMXX_TXX_STAT0(port_num, 1));
		gmx_tx_stat1.u64 = cvmx_read_csr(CVMX_GMXX_TXX_STAT1(port_num, 1));
		gmx_tx_stat2.u64 = cvmx_read_csr(CVMX_GMXX_TXX_STAT2(port_num, 1));
		gmx_tx_stat3.u64 = cvmx_read_csr(CVMX_GMXX_TXX_STAT3(port_num, 1));
		#if 0
		gmx_tx_stat4.u64 = cvmx_read_csr(CVMX_GMXX_TXX_STAT4(port_num, 1));
		gmx_tx_stat5.u64 = cvmx_read_csr(CVMX_GMXX_TXX_STAT5(port_num, 1));
		gmx_tx_stat6.u64 = cvmx_read_csr(CVMX_GMXX_TXX_STAT6(port_num, 1));
		gmx_tx_stat7.u64 = cvmx_read_csr(CVMX_GMXX_TXX_STAT7(port_num, 1));
		#endif
		gmx_tx_stat8.u64 = cvmx_read_csr(CVMX_GMXX_TXX_STAT8(port_num, 1));
		gmx_tx_stat9.u64 = cvmx_read_csr(CVMX_GMXX_TXX_STAT9(port_num, 1));
		//printk("dev %s \n",dev->name);
		//printk("status->rx_packets=%llu,status->multicast=%llu.\n",status->rx_packets,status->multicast);
	}
	else
	{
	}

	/* get rx stats */
	port_counter->rx.uncastframes= status->rx_packets - status->multicast; //pkts - bcst - mcst
	port_counter->rx.bcastframes= 0;
	port_counter->rx.mcastframes= status->multicast;

	//port_counter->rx.CRCerrors	  = pip_stat_inb_errsx.s.errs; /* imitate the eth-driver, but I think it should be (stat7.s.fcs), by baolc */
	port_counter->rx.errorframe= status->rx_frame_errors; /* imitate the eth-driver, but I think it should be (pip_stat_inb_errsx.s.errs), by baolc */
	port_counter->rx.dropped= status->rx_dropped; 
	port_counter->rx.fifooverruns	= gmx_rx_stat_overrun_pkts.s.cnt; /* overruns pkts, data read from gmx_rx_stats_drp CSRs */
	//port_counter->rx.carrier		= 0; /* can not find data */
	//port_counter->rx.collision	= 0; /* can not find data */
	port_counter->rx.goodbytesl 	= (status->rx_bytes  & 0xffffffff);//(status->inb_errors & 0xffffffff); /* low 32 bit */
	port_counter->rx.goodbytesh 	= ((status->rx_bytes >> 32) & 0xffff);//((status->inb_errors >> 32) & 0xffff); /* high 16 bit */
	port_counter->rx.badbytes		= 0; /* can not find data */
	port_counter->rx.fcframe		= gmx_rx_stat_fc_pkts.s.cnt;
	port_counter->rx.jabber 		= 0;
	port_counter->rx.underSizepkt	= 0;
	port_counter->rx.overSizepkt	= 0;
	port_counter->rx.fragments		= 0;

    /* get tx stats */
	//port_counter->tx.packets      = gmx_tx_stat3.s.pkts;
	//port_counter->tx.frame        = gmx_tx_stat3.s.pkts; /* just the same as pkts */
	//port_counter->tx.errors		  = 0; /* can not find data */
	//port_counter->tx.dropped      = gmx_tx_stat0.s.xsdef + gmx_tx_stat0.s.xscol; /* drp pkts because of deferrals and collision */
	//port_counter->tx.overruns     = 0; /* can not find data */
	
	port_counter->tx.goodbytesl   = status->tx_bytes & 0xffffffff; /* low 32 bit */
	port_counter->tx.goodbytesh   = (status->tx_bytes >> 32) & 0xffff; /* high 16 bit */	
	port_counter->tx.uncastframe  = status->tx_packets;//status->tx_packets - status->;
	port_counter->tx.mcastframe   = gmx_tx_stat8.s.mcst;
	port_counter->tx.bcastframe   = gmx_tx_stat8.s.bcst;
	port_counter->tx.fcframe      = 0;//gmx_tx_stat9.s.ctl;
	port_counter->tx.crcerror_fifooverrun = 0; /* can not find data */
	port_counter->tx.excessiveCollision = 0;
	port_counter->tx.sentMutiple = 0;
	port_counter->tx.sent_deferred = 0;

	/*not support for counters below */
	portOctCounter->badCRC = 0;
	portOctCounter->collision = 0;
	portOctCounter->late_collision = 0;
	portOctCounter->b64oct = 0;
	portOctCounter->b64oct127 = 0;
	portOctCounter->b128oct255 = 0;
	portOctCounter->b256oct511 = 0;
	portOctCounter->b512oct21023 = 0;
	portOctCounter->b1024oct2max = 0;
	return 0;
}

#if 0
/**
  * Read board eth port's statistics.
  * @param port_num: port number.
  * @param clear: if clear=1, clear all statistics after reading CSRs.
  * @param port_counter: all statistics data.
  * @return: 0 for success. -1 for failure.
  */
int do_read_board_eth_port_stats
(
	int port_num, 
	int clear, 
	struct npd_port_counter* port_counter,
	struct port_oct_s* portOctCounter
)
{
	/* PIP stats data */
	cvmx_pip_stat_ctl_t pip_stat_ctl; /* for clear PIP stats CSRs */
    cvmx_pip_stat0_prtx_t stat0;
    cvmx_pip_stat1_prtx_t stat1;
    cvmx_pip_stat2_prtx_t stat2;
    cvmx_pip_stat3_prtx_t stat3;
    cvmx_pip_stat4_prtx_t stat4;
    cvmx_pip_stat5_prtx_t stat5;
    cvmx_pip_stat6_prtx_t stat6;
    cvmx_pip_stat7_prtx_t stat7;
    cvmx_pip_stat8_prtx_t stat8;
    cvmx_pip_stat9_prtx_t stat9;
    cvmx_pip_stat_inb_pktsx_t pip_stat_inb_pktsx;
    cvmx_pip_stat_inb_octsx_t pip_stat_inb_octsx;
    cvmx_pip_stat_inb_errsx_t pip_stat_inb_errsx;

	/* GMX rx stats data */ 
	cvmx_gmxx_rxx_stats_octs_t  gmx_rx_stat_goodbytes;
    cvmx_gmxx_rxx_stats_pkts_ctl_t  gmx_rx_stat_fc_pkts;
	cvmx_gmxx_rxx_stats_pkts_drp_t  gmx_rx_stat_overrun_pkts;

	/* GMX tx stats data */
	cvmx_gmxx_txx_stat0_t  gmx_tx_stat0;
	cvmx_gmxx_txx_stat1_t  gmx_tx_stat1;
	cvmx_gmxx_txx_stat2_t  gmx_tx_stat2;
	cvmx_gmxx_txx_stat3_t  gmx_tx_stat3;
	cvmx_gmxx_txx_stat4_t  gmx_tx_stat4;
	cvmx_gmxx_txx_stat5_t  gmx_tx_stat5;
	cvmx_gmxx_txx_stat6_t  gmx_tx_stat6;
	cvmx_gmxx_txx_stat7_t  gmx_tx_stat7;
	cvmx_gmxx_txx_stat8_t  gmx_tx_stat8;
	cvmx_gmxx_txx_stat9_t  gmx_tx_stat9;

	if (port_num < 0 || port_num > 3 || port_counter == NULL || portOctCounter == NULL)
		return -1;

	/* if clear stats */
	if (clear != 1)
	{
		clear = 0; // 0 for not clear stats CSRs, 1 for clear them
	}
	pip_stat_ctl.u64 = 0;
    pip_stat_ctl.s.rdclr = clear;
    cvmx_write_csr(CVMX_PIP_STAT_CTL, pip_stat_ctl.u64); /* ctl pip stat */


if(7 == cvm_oct_get_product_type())
	{
		cvmx_write_csr(CVMX_GMXX_RXX_STATS_CTL(port_num, 1), pip_stat_ctl.u64); /* ctl gmx rx stat */
		cvmx_write_csr(CVMX_GMXX_TXX_STATS_CTL(port_num, 1), pip_stat_ctl.u64); /* ctl gmx tx stat */

	    /* read pip stats data */ /* Note the pip port is 0~35, board rgmii port(interface 1)'s range is 16~19 */
	    stat0.u64 = cvmx_read_csr(CVMX_PIP_STAT0_PRTX(port_num + 16));
	    stat1.u64 = cvmx_read_csr(CVMX_PIP_STAT1_PRTX(port_num + 16));
	    stat2.u64 = cvmx_read_csr(CVMX_PIP_STAT2_PRTX(port_num + 16));
	    stat3.u64 = cvmx_read_csr(CVMX_PIP_STAT3_PRTX(port_num + 16));
		if (clear == 1)
		{
			stat4.u64 = cvmx_read_csr(CVMX_PIP_STAT4_PRTX(port_num + 16));
			stat5.u64 = cvmx_read_csr(CVMX_PIP_STAT5_PRTX(port_num + 16));
			stat6.u64 = cvmx_read_csr(CVMX_PIP_STAT6_PRTX(port_num + 16));
		}
	    stat7.u64 = cvmx_read_csr(CVMX_PIP_STAT7_PRTX(port_num + 16));
	    stat8.u64 = cvmx_read_csr(CVMX_PIP_STAT8_PRTX(port_num + 16));
	    stat9.u64 = cvmx_read_csr(CVMX_PIP_STAT9_PRTX(port_num + 16));
	    pip_stat_inb_pktsx.u64 = cvmx_read_csr(CVMX_PIP_STAT_INB_PKTSX(port_num + 16));
	    pip_stat_inb_octsx.u64 = cvmx_read_csr(CVMX_PIP_STAT_INB_OCTSX(port_num + 16));
	    pip_stat_inb_errsx.u64 = cvmx_read_csr(CVMX_PIP_STAT_INB_ERRSX(port_num + 16));

	    /* read gmx rx stats data */
		gmx_rx_stat_goodbytes.u64    = cvmx_read_csr(CVMX_GMXX_RXX_STATS_OCTS(port_num, 1));
		gmx_rx_stat_fc_pkts.u64      = cvmx_read_csr(CVMX_GMXX_RXX_STATS_PKTS_CTL(port_num, 1));
		gmx_rx_stat_overrun_pkts.u64 = cvmx_read_csr(CVMX_GMXX_RXX_STATS_PKTS_DRP(port_num, 1));
		if (clear == 1)
		{
			cvmx_read_csr(CVMX_GMXX_RXX_STATS_OCTS_CTL(port_num, 1)); /* read these CSRs just for clear them */
			cvmx_read_csr(CVMX_GMXX_RXX_STATS_OCTS_DMAC(port_num, 1)); /* read these CSRs just for clear them */
			cvmx_read_csr(CVMX_GMXX_RXX_STATS_OCTS_DRP(port_num, 1)); /* read these CSRs just for clear them */
			cvmx_read_csr(CVMX_GMXX_RXX_STATS_PKTS(port_num, 1)); /* read these CSRs just for clear them */
			cvmx_read_csr(CVMX_GMXX_RXX_STATS_PKTS_BAD(port_num, 1)); /* read these CSRs just for clear them */
			cvmx_read_csr(CVMX_GMXX_RXX_STATS_PKTS_DMAC(port_num, 1)); /* read these CSRs just for clear them */
		}

		/* read gmx tx stats data */
	    gmx_tx_stat0.u64 = cvmx_read_csr(CVMX_GMXX_TXX_STAT0(port_num, 1));
		gmx_tx_stat1.u64 = cvmx_read_csr(CVMX_GMXX_TXX_STAT1(port_num, 1));
		gmx_tx_stat2.u64 = cvmx_read_csr(CVMX_GMXX_TXX_STAT2(port_num, 1));
		gmx_tx_stat3.u64 = cvmx_read_csr(CVMX_GMXX_TXX_STAT3(port_num, 1));
		if (clear == 1)
		{
			gmx_tx_stat4.u64 = cvmx_read_csr(CVMX_GMXX_TXX_STAT4(port_num, 1));
			gmx_tx_stat5.u64 = cvmx_read_csr(CVMX_GMXX_TXX_STAT5(port_num, 1));
			gmx_tx_stat6.u64 = cvmx_read_csr(CVMX_GMXX_TXX_STAT6(port_num, 1));
			gmx_tx_stat7.u64 = cvmx_read_csr(CVMX_GMXX_TXX_STAT7(port_num, 1));
		}
		gmx_tx_stat8.u64 = cvmx_read_csr(CVMX_GMXX_TXX_STAT8(port_num, 1));
		gmx_tx_stat9.u64 = cvmx_read_csr(CVMX_GMXX_TXX_STAT9(port_num, 1));



	}
else
	{
		cvmx_write_csr(CVMX_GMXX_RXX_STATS_CTL(port_num, 0), pip_stat_ctl.u64); /* ctl gmx rx stat */
		cvmx_write_csr(CVMX_GMXX_TXX_STATS_CTL(port_num, 0), pip_stat_ctl.u64); /* ctl gmx tx stat */

	    /* read pip stats data */ /* Note the pip port is 0~35, board rgmii port(interface 1)'s range is 16~19 */
	    stat0.u64 = cvmx_read_csr(CVMX_PIP_STAT0_PRTX(port_num));
	    stat1.u64 = cvmx_read_csr(CVMX_PIP_STAT1_PRTX(port_num));
	    stat2.u64 = cvmx_read_csr(CVMX_PIP_STAT2_PRTX(port_num));
	    stat3.u64 = cvmx_read_csr(CVMX_PIP_STAT3_PRTX(port_num));
		if (clear == 1)
		{
			stat4.u64 = cvmx_read_csr(CVMX_PIP_STAT4_PRTX(port_num));
			stat5.u64 = cvmx_read_csr(CVMX_PIP_STAT5_PRTX(port_num));
			stat6.u64 = cvmx_read_csr(CVMX_PIP_STAT6_PRTX(port_num));
		}
	    stat7.u64 = cvmx_read_csr(CVMX_PIP_STAT7_PRTX(port_num));
	    stat8.u64 = cvmx_read_csr(CVMX_PIP_STAT8_PRTX(port_num));
	    stat9.u64 = cvmx_read_csr(CVMX_PIP_STAT9_PRTX(port_num));
	    pip_stat_inb_pktsx.u64 = cvmx_read_csr(CVMX_PIP_STAT_INB_PKTSX(port_num));
	    pip_stat_inb_octsx.u64 = cvmx_read_csr(CVMX_PIP_STAT_INB_OCTSX(port_num));
	    pip_stat_inb_errsx.u64 = cvmx_read_csr(CVMX_PIP_STAT_INB_ERRSX(port_num));

	    /* read gmx rx stats data */
		gmx_rx_stat_goodbytes.u64    = cvmx_read_csr(CVMX_GMXX_RXX_STATS_OCTS(port_num, 0));
		gmx_rx_stat_fc_pkts.u64      = cvmx_read_csr(CVMX_GMXX_RXX_STATS_PKTS_CTL(port_num, 0));
		gmx_rx_stat_overrun_pkts.u64 = cvmx_read_csr(CVMX_GMXX_RXX_STATS_PKTS_DRP(port_num, 0));
		if (clear == 1)
		{
			cvmx_read_csr(CVMX_GMXX_RXX_STATS_OCTS_CTL(port_num, 0)); /* read these CSRs just for clear them */
			cvmx_read_csr(CVMX_GMXX_RXX_STATS_OCTS_DMAC(port_num, 0)); /* read these CSRs just for clear them */
			cvmx_read_csr(CVMX_GMXX_RXX_STATS_OCTS_DRP(port_num, 0)); /* read these CSRs just for clear them */
			cvmx_read_csr(CVMX_GMXX_RXX_STATS_PKTS(port_num, 0)); /* read these CSRs just for clear them */
			cvmx_read_csr(CVMX_GMXX_RXX_STATS_PKTS_BAD(port_num, 0)); /* read these CSRs just for clear them */
			cvmx_read_csr(CVMX_GMXX_RXX_STATS_PKTS_DMAC(port_num, 0)); /* read these CSRs just for clear them */
		}

		/* read gmx tx stats data */
	    gmx_tx_stat0.u64 = cvmx_read_csr(CVMX_GMXX_TXX_STAT0(port_num, 0));
		gmx_tx_stat1.u64 = cvmx_read_csr(CVMX_GMXX_TXX_STAT1(port_num, 0));
		gmx_tx_stat2.u64 = cvmx_read_csr(CVMX_GMXX_TXX_STAT2(port_num, 0));
		gmx_tx_stat3.u64 = cvmx_read_csr(CVMX_GMXX_TXX_STAT3(port_num, 0));
		if (clear == 1)
		{
			gmx_tx_stat4.u64 = cvmx_read_csr(CVMX_GMXX_TXX_STAT4(port_num, 0));
			gmx_tx_stat5.u64 = cvmx_read_csr(CVMX_GMXX_TXX_STAT5(port_num, 0));
			gmx_tx_stat6.u64 = cvmx_read_csr(CVMX_GMXX_TXX_STAT6(port_num, 0));
			gmx_tx_stat7.u64 = cvmx_read_csr(CVMX_GMXX_TXX_STAT7(port_num, 0));
		}
		gmx_tx_stat8.u64 = cvmx_read_csr(CVMX_GMXX_TXX_STAT8(port_num, 0));
		gmx_tx_stat9.u64 = cvmx_read_csr(CVMX_GMXX_TXX_STAT9(port_num, 0));



	}
	/* get rx stats */
    port_counter->rx.uncastframes= pip_stat_inb_pktsx.s.pkts - stat3.s.bcst - stat3.s.mcst; //pkts - bcst - mcst
	port_counter->rx.bcastframes= stat3.s.bcst;
	port_counter->rx.mcastframes= stat3.s.mcst;
	//port_counter->rx.CRCerrors      = pip_stat_inb_errsx.s.errs; /* imitate the eth-driver, but I think it should be (stat7.s.fcs), by baolc */
	port_counter->rx.errorframe= stat7.s.fcs; /* imitate the eth-driver, but I think it should be (pip_stat_inb_errsx.s.errs), by baolc */
	//port_counter->rx.dropped= stat0.s.drp_pkts; 
	port_counter->rx.fifooverruns	= gmx_rx_stat_overrun_pkts.s.cnt; /* overruns pkts, data read from gmx_rx_stats_drp CSRs */
	//port_counter->rx.carrier		= 0; /* can not find data */
	//port_counter->rx.collision   	= 0; /* can not find data */
	port_counter->rx.goodbytesl		= (gmx_rx_stat_goodbytes.s.cnt & 0xffffffff); /* low 32 bit */
	port_counter->rx.goodbytesh     = ((gmx_rx_stat_goodbytes.s.cnt >> 32) & 0xffff); /* high 16 bit */
	port_counter->rx.badbytes		= 0; /* can not find data */
	port_counter->rx.fcframe        = gmx_rx_stat_fc_pkts.s.cnt;
	port_counter->rx.jabber         = stat9.s.jabber;
	port_counter->rx.underSizepkt	= stat8.s.undersz;
	port_counter->rx.overSizepkt	= stat9.s.oversz;
	port_counter->rx.fragments      = stat8.s.frag;

    /* get tx stats */
	//port_counter->tx.packets      = gmx_tx_stat3.s.pkts;
	//port_counter->tx.frame        = gmx_tx_stat3.s.pkts; /* just the same as pkts */
	//port_counter->tx.errors		  = 0; /* can not find data */
	//port_counter->tx.dropped      = gmx_tx_stat0.s.xsdef + gmx_tx_stat0.s.xscol; /* drp pkts because of deferrals and collision */
	//port_counter->tx.overruns     = 0; /* can not find data */
	port_counter->tx.goodbytesl   = gmx_tx_stat2.s.octs & 0xffffffff; /* low 32 bit */
	port_counter->tx.goodbytesh   = (gmx_tx_stat2.s.octs >> 32) & 0xffff; /* high 16 bit */
	port_counter->tx.uncastframe  = gmx_tx_stat3.s.pkts - gmx_tx_stat8.s.mcst - gmx_tx_stat8.s.bcst;
	port_counter->tx.mcastframe   = gmx_tx_stat8.s.mcst;
	port_counter->tx.bcastframe   = gmx_tx_stat8.s.bcst;
	port_counter->tx.fcframe      = gmx_tx_stat9.s.ctl;
	port_counter->tx.crcerror_fifooverrun = 0; /* can not find data */
	port_counter->tx.excessiveCollision = 0;
	port_counter->tx.sentMutiple = 0;
	port_counter->tx.sent_deferred = 0;

	/*not support for counters below */
	portOctCounter->badCRC = 0;
	portOctCounter->collision = 0;
	portOctCounter->late_collision = 0;
	portOctCounter->b64oct = 0;
	portOctCounter->b64oct127 = 0;
	portOctCounter->b128oct255 = 0;
	portOctCounter->b256oct511 = 0;
	portOctCounter->b512oct21023 = 0;
	portOctCounter->b1024oct2max = 0;
	return 0;
}

#endif

/*
	@return 
			7:	auteX 7000 product.
			5:	auteU 5000 product.
			4:	auteU 4624 product.
			3:	auteU 3000 product.


*/

unsigned char bm_cvm_oct_get_product_type(void)
{
	unsigned char product_num = 0;
	product_num = cvmx_read64_uint8(CPLD1_BASE_ADDR+CPLD_PRODUCT_CTL+ base_addr64);
	return product_num&0xf;
}


/*
	CPLD reg 0x10 
	bit: [7]		[6]		[5]		[4]		[3]		[2]		[1]		[0]
		XFP1	XFP0	CARD1	CARD0	SFP3	SFP2	SFP1	SFP0
		BIT[4] ==0	CARD0 is ON.
		BIT[4] ==1	CARD0 is OFF.
*/

unsigned char bm_cvm_query_card_on(int card_Num)
{
	unsigned char tmp;
	if (card_Num > 1)
		return 0xff;
	tmp = cvmx_read64_uint8(CPLD1_BASE_ADDR + CPLD_CARD_ON + base_addr64);
	return ((tmp >> (card_Num + 4)) & 0x1);


}


/*
	CARD0 REG 0X24  BIT[3:2]
	CARD1 REG 0X25  BIT[3:2]
	01 ----- AX51-XFP
	10 ----- AX51-GTX
	11 ----- AX51-SFP
*/
unsigned char bm_cvm_query_card_type(int card_Num)
{
	unsigned char tmp;
	if (card_Num > 1)
		return -1;
	tmp = cvmx_read64_uint8(CPLD1_BASE_ADDR + CPLD_CARD_TYPE(card_Num)+ base_addr64);
	
	return ((tmp >> 2) & 0x3);

}

