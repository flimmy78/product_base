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


#include "octeon_req_perf.h"

extern struct __test_info  tinfo;

extern struct __seq_params  tseq;

extern int run_all_tests;



#ifndef __KERNEL__
int
get_dma_mode(char *dma_string)
{
	if(!strncasecmp(dma_string, "direct", 6))
		tinfo.dma_mode = OCTEON_DMA_DIRECT;
	else if(!strncasecmp(dma_string, "gather", 6))
		tinfo.dma_mode = OCTEON_DMA_GATHER;
	else if(!strncasecmp(dma_string, "scatter_gather", 14))
		tinfo.dma_mode = OCTEON_DMA_SCATTER_GATHER;
	else if(!strncasecmp(dma_string, "scatter", 7))
		tinfo.dma_mode = OCTEON_DMA_SCATTER;
	else if(!strncasecmp(dma_string, "random", 6))
		tinfo.dma_mode = RANDOM;
	else
		return -1;

	return 0;
}






int
get_resp_order(char *resp_order_string)
{
	if(!strncasecmp(resp_order_string, "ordered", 7))
		tinfo.resp_order = OCTEON_RESP_ORDERED;
	else if(!strncasecmp(resp_order_string, "unordered", 9))
		tinfo.resp_order = OCTEON_RESP_UNORDERED;
	else if(!strncasecmp(resp_order_string, "noresponse", 10))
		tinfo.resp_order = OCTEON_RESP_NORESPONSE;
	else if(!strncasecmp(resp_order_string, "random", 6))
		tinfo.resp_order = RANDOM;
	else
		return -1;

	return 0;
}





int
get_resp_mode(char *resp_mode_string)
{
	if(!strncasecmp(resp_mode_string, "blocking", 8))
		tinfo.resp_mode = OCTEON_RESP_BLOCKING;
	else if(!strncasecmp(resp_mode_string, "nonblocking", 11))
		tinfo.resp_mode = OCTEON_RESP_NON_BLOCKING;
	else if(!strncasecmp(resp_mode_string, "random", 6))
		tinfo.resp_mode = RANDOM;
	else
		return -1;

	return 0;
}





int
get_iq_no(char *iq_no_string)
{
	if(!strncasecmp(iq_no_string, "random", 6)) {
		tinfo.iq_no = RANDOM;
	} else  {
		tinfo.iq_no = strtol(iq_no_string, NULL ,10);
	}
	return 0;
}





int
get_octeon_no(char *octeon_no_string)
{
	if(!strncasecmp(octeon_no_string, "random", 6)) {
		tinfo.octeon_id = RANDOM;
	} else  {
		tinfo.octeon_id = strtol(octeon_no_string, NULL ,10);
	}
	return 0;
}




int
get_pkt_count(char *pkt_cnt_string)
{
	tinfo.pkt_cnt = strtol(pkt_cnt_string, NULL ,10);
	return 0;
}



int
get_datagen_type(char *datagen)
{
	if(!strncasecmp(datagen, "random", 6)) {
		tinfo.datagen = RANDOM;
		return 0;
	} else {
		if(!strncasecmp(datagen, "sequential", 10)) {
			tinfo.datagen = SEQUENTIAL;
			return 0;
		}
	}

	return -1;
}




int
get_buf_count(char **bs_arg,  int *idx)
{
	(*idx)++;
	if(!strncasecmp(bs_arg[*idx], "min", 3)) {
		++(*idx);
		tinfo.min_bufs = strtol(bs_arg[*idx], NULL ,10);
		return 0;
	} else if(!strncasecmp(bs_arg[*idx], "max", 3)) {
		++(*idx);
		tinfo.max_bufs = strtol(bs_arg[*idx], NULL ,10);
		return 0;
	}

	return -1;
}




int
get_buf_sizes(char **bs_arg,  int *idx)
{
	(*idx)++;
	if(!strncasecmp(bs_arg[*idx], "min", 3)) {
		++(*idx);
		tinfo.min_datasize = strtol(bs_arg[*idx], NULL ,10);
		return 0;
	} else if(!strncasecmp(bs_arg[*idx], "max", 3)) {
		++(*idx);
		tinfo.max_datasize = strtol(bs_arg[*idx], NULL ,10);
		return 0;
	}

	return -1;
}

#endif




int
process_and_verify_args(void)
{
	if(tinfo.iq_no > 16) {
		PRINT_ERR("Invalid IQ Number: %d\n", tinfo.iq_no);
		return 1;
	}

	if(tinfo.octeon_id > 1) {
		PRINT_ERR("Invalid Octeon Device Number: %d\n", tinfo.octeon_id);
		return 1;
	}

	if(tinfo.max_bufs > MAX_BUFCNT) {
		PRINT_ERR("Buffer count (%d) exceeds max (%d)\n",
		       tinfo.max_bufs, MAX_BUFCNT);
		return 1;
	}

	if(tinfo.min_bufs > tinfo.max_bufs) {
		PRINT_ERR("Min buffer count (%d) exceeds max (%d)\n",
		       tinfo.min_bufs, tinfo.max_bufs);
		return 1;
	}

	if(tinfo.min_datasize > tinfo.max_datasize) {
		PRINT_ERR("Min data size  (%d) exceeds max (%d)\n",
		       tinfo.min_datasize, tinfo.max_datasize);
		return 1;
	}

	if(tinfo.pkt_cnt < 0) {
		PRINT_ERR("Invalid Packet count: %d\n", tinfo.pkt_cnt);
		return 1;
	}

	if(tinfo.datagen == SEQUENTIAL) {
		tseq.bufcnt.curr = tinfo.min_bufs;
		tseq.bufcnt.incr = (tinfo.max_bufs - tinfo.min_bufs + 1);
		if(MAX_REQUESTS < tseq.bufcnt.incr)
			tseq.bufcnt.incr += MAX_REQUESTS;
		tseq.bufcnt.incr /= MAX_REQUESTS;
		if(tseq.bufcnt.incr == 0) tseq.bufcnt.incr = 1;

		tseq.datasize.curr = tinfo.min_datasize;
		tseq.datasize.incr = (tinfo.max_datasize - tinfo.min_datasize + 1);
		if(MAX_REQUESTS < tseq.datasize.incr)
			tseq.datasize.incr += MAX_REQUESTS;
		tseq.datasize.incr /= MAX_REQUESTS;
		if(tseq.datasize.incr == 0) tseq.datasize.incr = 1;
	}

	return 0;
}


#ifndef __KERNEL__

void
print_usage( int verbose)
{
	PRINT_MSG("./oct_test [-d <dma_mode> ] [-r <resp_order> ] [-m <resp_mode> ]\n");
	PRINT_MSG("           [-q <input queue> ]  [-o <octeon id> ] [-n <pkt_count> ]\n");
	PRINT_MSG("           [-t <datagen_type> ]\n");
	PRINT_MSG("           [-s {min | max} <request data size> ]\n");
	PRINT_MSG("           [-c {min | max} <buffer count> ]\n");
	PRINT_MSG("\n./oct_test -h  Prints this help message\n");

	if (verbose) {
		PRINT_MSG("\n Supported values for arguments\n");
		PRINT_MSG("   dma_mode: direct, gather, scatter, scatter_gather, random\n");
		PRINT_MSG("   resp_order: unordered, noresponse, random\n");
		PRINT_MSG("   resp_mode: blocking, nonblocking, random\n");
		PRINT_MSG("   datagen_type: random, sequential\n");
		PRINT_MSG("   request data size & buffer count values depend on the DMA type used\n");
	} else {
		PRINT_MSG(" use -v option for more verbose help\n");
	}
	PRINT_MSG("\n");
}



int
parse_args(int argc, char **argv)
{
	int idx = 1, help = 0, verbose = 0;

	if(argc == 1) {
		PRINT_MSG("Using default settings\n");
	} else {

		while(idx < argc) {

			if(argv[idx][0] == '-') {
				switch(argv[idx][1]) {
					case 'd': if( get_dma_mode(argv[++idx]) < 0)
					             goto parse_error;
					    break;
					case 'r': if( get_resp_order(argv[++idx]) < 0)
					             goto parse_error;
					    break;
					case 'm': if( get_resp_mode(argv[++idx]) < 0)
					             goto parse_error;
					    break;
					case 'q': if( get_iq_no(argv[++idx]) < 0)
					             goto parse_error;
					    break;
					case 'o': if( get_octeon_no(argv[++idx]) < 0)
					             goto parse_error;
					    break;
					case 'n': if( get_pkt_count(argv[++idx]) < 0)
					             goto parse_error;
					    break;
					case 't': if(get_datagen_type(argv[++idx]) < 0)
					             goto parse_error;
					    break;
					case 's': if(get_buf_sizes(argv, &idx) < 0)
					             goto parse_error;
					    break;
					case 'c': if(get_buf_count(argv, &idx) < 0)
					             goto parse_error;
					    break;
					case 'h':  help = 1;
					    break;
					case 'v':  verbose = 1;
					    break;
					case 'a':  run_all_tests = 1;
						break;
					default:  PRINT_ERR("Unknown option: %s\n", argv[idx]);
				          return -EINVAL;
				}
				idx++;
			} else {
				goto parse_error;
			}
		}
	}

	if(help) {
		print_usage(verbose);
		return -1;
	}

	return 0;

parse_error:
	PRINT_ERR("Invalid Option %s\n", argv[idx]);
	return -EINVAL;
}

#endif


/* $Id: parse_args.c 53786 2010-10-08 22:09:32Z panicker $ */

