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




#include "octeon_main.h"
#include "octeon_macros.h"
#include "octeon_debug.h"
#include "cavium_proc.h"
#include "octeon_hw.h"

#include "octeon_reg_defs.h"
#include "octeon_mem_ops.h"



#ifdef CAVIUM_DEBUG
static char debug_level_str[5][30] =
	{ "only error messages",
	  " register values ",
	  "error & debug messages",
	  "error, flow & debug messages",
	  "all messages"  };
#endif

/* Buffer pool statistics maintained in another file. */
extern uint32_t buffer_stats[], alloc_buffer_stats[],fragment_buf_stats[];
extern uint32_t other_pools[];


int  cn3xxx_read_csrreg_buf(octeon_device_t  *oct, char *buf, int bufsize);
int  cn3xxx_read_npireg_buf(octeon_device_t  *oct, char *buf, int bufsize);
int  cn3xxx_read_configreg_buf(octeon_device_t  *oct, char *buf, int bufsize);

int  cn56xx_read_csrreg_buf(octeon_device_t  *oct, char *buf, int bufsize);
int  cn56xx_read_configreg_buf(octeon_device_t  *oct, char *buf, int bufsize);

int  cn63xx_read_csrreg_buf(octeon_device_t  *oct, char *buf, int bufsize);
int  cn63xx_read_configreg_buf(octeon_device_t  *oct, char *buf, int bufsize);

int  cn68xx_read_csrreg_buf(octeon_device_t  *oct, char *buf, int bufsize);
int  cn68xx_read_configreg_buf(octeon_device_t  *oct, char *buf, int bufsize);





#define REGBUFSIZE (4096)


/* Temporary buffers used for each Octeon device when its proc file is read. */
static struct oct_proc_buf {

	uint64_t  input_bytes[32];
	uint64_t  output_bytes[32];
	uint64_t  diff_bytes[32];
	char      csrbuf[REGBUFSIZE];
	char      npibuf[REGBUFSIZE];
	char      configbuf[REGBUFSIZE];
	char      statusbuf[REGBUFSIZE];
	int       csrbuflen;
	int       npibuflen;
	int       configbuflen;
	int       statusbuflen;
} *proc_buf[MAX_OCTEON_DEVICES];








#ifdef CAVIUM_DEBUG
static int
proc_write_debug_level(struct file *file, const char *buffer,
                       unsigned long count, void *data)
{
   char str[10], *strend;


   CVM_MOD_INC_USE_COUNT;

   if(cavium_copy_in(str, buffer, count)) {
      cavium_error("cavium_copy_in failed\n");
      CVM_MOD_DEC_USE_COUNT;
      return -EFAULT;
   }
   str[count] = '\0';
   octeon_debug_level = simple_strtoul(str, &strend, 0); 
   cavium_print(PRINT_MSG,"Octeon Debug Level set to print %s\n",
                    debug_level_str[octeon_debug_level]);

   CVM_MOD_DEC_USE_COUNT;


   return count;
}







static int
proc_read_debug_level(char *page, char **start, off_t off, int count,
                      int *eof, void *data)
{
   int len = 0;

   CVM_MOD_INC_USE_COUNT;
   proc_print(page, len, "Octeon debug level: %d [%s]\n", octeon_debug_level,
                                         debug_level_str[octeon_debug_level]);
   CVM_MOD_DEC_USE_COUNT;
   *eof = 1;
   return len;
}
#endif













static int
proc_read_stats(char *page, char **start, off_t off,
                     int count,int *eof, void *data)
{
   int                     len = 0, i;
   uint64_t                total_diff_bytes = 0;
   octeon_device_t        *octeon_dev;
   struct oct_proc_buf    *pbuf;

   octeon_dev = (octeon_device_t *)data;
   pbuf = proc_buf[octeon_dev->octeon_id];

   CVM_MOD_INC_USE_COUNT;

   proc_print(page, len, "Octeon (Status: %s)\n",
              get_oct_state_string(&octeon_dev->status) );

#ifdef CAVIUM_DEBUG
   proc_print(page, len, "Debug Level: %d\n", octeon_debug_level);
#endif

   proc_print(page, len, "jiffies: %lu\n", jiffies);

#ifdef USE_BUFFER_POOL
   proc_print(page, len, "\n--Buffer Pool----   ");
   for(i = 0; i < BUF_POOLS; i++)
     proc_print(page, len, " Pool %d ", i);   
   proc_print(page, len, "\n  Max  buffers   :  ");
   for(i = 0; i < BUF_POOLS; i++)
     proc_print(page, len, " %5d  ", buffer_stats[i]);
   proc_print(page, len, "\n  allocated bufs :  ");
   for(i = 0; i < BUF_POOLS; i++)
     proc_print(page, len, " %5d  ", alloc_buffer_stats[i]);
   proc_print(page, len, "\n  fragmented bufs:  ");
   for(i = 0; i < BUF_POOLS; i++)
     proc_print(page, len, " %5d  ", fragment_buf_stats[i]);
   proc_print(page, len, "\n  other pools    :  ");
   for(i = 0; i < BUF_POOLS; i++)
     proc_print(page, len, " %5d  ", other_pools[i]);
   proc_print(page, len,
   "\n____________________________________________________________________\n");
#endif


   proc_print(page, len,  "--Instruction Queue--   ");

   for(i = 0; i < octeon_dev->num_iqs; i++)
       proc_print(page, len,"    IQ %d    ", i);

   proc_print(page, len,"\n Instr processed   :");
   for(i = 0; i < octeon_dev->num_iqs; i++)
       proc_print(page,len,"  %8llu ",
                CVM_CAST64(octeon_dev->instr_queue[i]->stats.instr_processed));

   proc_print(page, len,"\n Instr dropped     :");
   for(i = 0; i < octeon_dev->num_iqs; i++)
       proc_print(page, len,"  %8llu  ",
                CVM_CAST64(octeon_dev->instr_queue[i]->stats.instr_dropped));

   proc_print(page, len,"\n Bytes Sent        :");
   for(i = 0; i < octeon_dev->num_iqs; i++)  {
       pbuf->diff_bytes[i] = octeon_dev->instr_queue[i]->stats.bytes_sent
                     - pbuf->input_bytes[i];
       proc_print(page, len,"  %8llu  ",
                  CVM_CAST64(octeon_dev->instr_queue[i]->stats.bytes_sent));
       pbuf->input_bytes[i] = octeon_dev->instr_queue[i]->stats.bytes_sent;
   }

   proc_print(page, len,"\n sgentry_sent      :");
   for(i = 0; i < octeon_dev->num_iqs;  i++)
       proc_print(page, len,"  %8llu  ",
                  CVM_CAST64(octeon_dev->instr_queue[i]->stats.sgentry_sent));

   proc_print(page, len,"\n Inst Cnt reg      :");
   for(i = 0; i < octeon_dev->num_iqs;  i++)  {
       proc_print(page, len,"  %8x  ",
                  OCTEON_READ32(octeon_dev->instr_queue[i]->inst_cnt_reg));
   }

   proc_print(page, len,"\n Bytes since last:");
   for(i = 0; i < octeon_dev->num_iqs;  i++) 
       proc_print(page, len,"  %8llu  ", CVM_CAST64(pbuf->diff_bytes[i]));

   proc_print(page, len,"\n Total Bytes since last:");
   total_diff_bytes = 0;
   for(i = 0; i < octeon_dev->num_iqs;  i++)
       total_diff_bytes += pbuf->diff_bytes[i];
   proc_print(page, len,"  %8llu  ", CVM_CAST64(total_diff_bytes));

   proc_print(page, len,
   "\n____________________________________________________________________\n");


   proc_print(page, len,  "--DROQ--               ");
   for(i = 0; i < octeon_dev->num_oqs; i++)
       proc_print(page, len,"   DROQ %d   ", i);


   proc_print(page, len,"\n Pkts Received      :");
   for(i = 0; i < octeon_dev->num_oqs; i++)
       proc_print(page,len,"  %8llu  ",
                  CVM_CAST64(octeon_dev->droq[i]->stats.pkts_received));

   proc_print(page, len,"\n Bytes Received     :");
   for(i = 0; i < octeon_dev->num_oqs;  i++) {
       pbuf->diff_bytes[i] = octeon_dev->droq[i]->stats.bytes_received
                       - pbuf->output_bytes[i];
       proc_print(page, len,"  %8llu  ",
                  CVM_CAST64(octeon_dev->droq[i]->stats.bytes_received));
       pbuf->output_bytes[i] = octeon_dev->droq[i]->stats.bytes_received;
   }

   proc_print(page, len,"\n Bytes since last:");
   for(i = 0; i < octeon_dev->num_oqs;  i++)
        proc_print(page, len,"  %8llu  ", CVM_CAST64(pbuf->diff_bytes[i]));

   total_diff_bytes = 0;
   for(i = 0; i < octeon_dev->num_oqs;  i++)
       total_diff_bytes += pbuf->diff_bytes[i];
   proc_print(page, len,"\n Total Bytes since last:");
   proc_print(page, len,"  %8llu  ", CVM_CAST64(total_diff_bytes));

   proc_print(page, len,"\n Pkts dropped ");

   proc_print(page, len,"\n        No dispatch :");
   for(i = 0; i < octeon_dev->num_oqs; i++)
       proc_print(page, len,"  %8llu  ",
                  CVM_CAST64(octeon_dev->droq[i]->stats.dropped_nodispatch));

   proc_print(page, len,"\n        No memory   :");
   for(i = 0; i < octeon_dev->num_oqs; i++)
       proc_print(page, len,"  %8llu  ",
                  CVM_CAST64(octeon_dev->droq[i]->stats.dropped_nomem));


   proc_print(page, len,"\n\n Pending free index: %u",
              octeon_dev->plist->free_index);
   proc_print(page, len,
   "\n____________________________________________________________________\n");

   if(octeon_dev->dma_ops.read_stats)
      len += octeon_dev->dma_ops.read_stats(octeon_dev->octeon_id, (page+len));

   CVM_MOD_DEC_USE_COUNT;
   *eof = 1;

   return len;
}












static int
fill_iq_status(octeon_device_t  *oct, int iq_no, char *buf, int bufsize)
{
	octeon_instr_queue_t  *iq = oct->instr_queue[iq_no];
	char  str[80];
	int   fill=0, filled_size = 0;

	fill = sprintf(str, "\nIQ %d\n-----------\n", iq_no);
	filled_size = cvm_copy_cond(buf, str, filled_size, fill, bufsize);

	fill = sprintf(str, ">> Configurations\n");
	filled_size = cvm_copy_cond(buf, str, filled_size, fill, bufsize);

	fill = sprintf(str,
	          "Entries: %d Instr Size: %d DB Fill Threshold: %d Timeout: %lu\n",
	            iq->max_count,
	            (oct->io_qmask.iq64B & (1 << iq_no))?64:32,
	            iq->fill_threshold, iq->db_timeout);
	filled_size = cvm_copy_cond(buf, str, filled_size, fill, bufsize);

	fill = sprintf(str, ">> Status\n");
	filled_size = cvm_copy_cond(buf, str, filled_size, fill, bufsize);

	fill = sprintf(str, "Instr Pending: %d Fill Count: %d Threshold: %d\n",
	           cavium_atomic_read(&iq->instr_pending), iq->fill_cnt,
	           iq->fill_threshold);
	filled_size = cvm_copy_cond(buf, str, filled_size, fill, bufsize);

	fill = sprintf(str, "Index  Write: %d  Read: %d Flush: %d\n",
	           iq->host_write_index, iq->octeon_read_index, iq->flush_index);
	filled_size = cvm_copy_cond(buf, str, filled_size, fill, bufsize);

	fill = sprintf(str, "Registers InstrCnt: %d DoorBell: %d\n",
	           OCTEON_READ32(iq->inst_cnt_reg),
	           OCTEON_READ32(iq->doorbell_reg));
	filled_size = cvm_copy_cond(buf, str, filled_size, fill, bufsize);

#if !defined(DISABLE_PCIE14425_ERRATAFIX)
	fill = sprintf(str, "BackPressure Hits: %d\n", iq->bp_hits);
	filled_size = cvm_copy_cond(buf, str, filled_size, fill, bufsize);
#endif


	return filled_size;
}




static int
fill_droq_status(octeon_device_t  *oct, int oq_no, char *buf, int bufsize)
{
	octeon_droq_t  *droq = oct->droq[oq_no];
	char  str[80];
	int   fill=0, filled_size = 0;

	fill = sprintf(str, "\nDROQ %d\n--------------\n", oq_no);
	filled_size = cvm_copy_cond(buf, str, filled_size, fill, bufsize);
	fill = sprintf(str, ">> Configuration\n");
	filled_size = cvm_copy_cond(buf, str, filled_size, fill, bufsize);

	fill = sprintf(str,
	     "Entries: %d Buffer Size: %d Pkts_per_intr: %d Refill Threshold: %d\n",
	       droq->max_count, droq->buffer_size, droq->pkts_per_intr,
	       droq->refill_threshold);
	filled_size = cvm_copy_cond(buf, str, filled_size, fill, bufsize);

	if(oct->chip_id <= OCTEON_CN58XX)
		fill = sprintf(str, "Intr Threshold Registers Pkt: %d Time: %d\n",
		               octeon_read_csr(oct, OCT_OQ_INTR_COUNT_LEV_REG(oq_no)),
		               octeon_read_csr(oct, OCT_OQ_INTR_TIME_LEV_REG(oq_no)) );


	if(oct->chip_id == OCTEON_CN56XX)
		fill = sprintf(str, "Intr Threshold Registers Pkt: %d Time: %d\n",
		               octeon_read_csr(oct, CN56XX_NPEI_OQ_INT_LEV_PKTS),
		               octeon_read_csr(oct, CN56XX_NPEI_OQ_INT_LEV_TIME) );

	if( (oct->chip_id == OCTEON_CN63XX) || (oct->chip_id == OCTEON_CN66XX) )
		fill = sprintf(str, "Intr Threshold Registers Pkt: %d Time: %d\n",
		               octeon_read_csr(oct, CN63XX_SLI_OQ_INT_LEVEL_PKTS),
		               octeon_read_csr(oct, CN63XX_SLI_OQ_INT_LEVEL_TIME) );

	if(oct->chip_id == OCTEON_CN68XX)
		fill = sprintf(str, "Intr Threshold Registers Pkt: %d Time: %d\n",
		               octeon_read_csr(oct, CN68XX_SLI_OQ_INT_LEVEL_PKTS),
		               octeon_read_csr(oct, CN68XX_SLI_OQ_INT_LEVEL_TIME) );

	filled_size = cvm_copy_cond(buf, str, filled_size, fill, bufsize);

	fill = sprintf(str, ">> Status\n");
	filled_size = cvm_copy_cond(buf, str, filled_size, fill, bufsize);

	fill = sprintf(str, "Packets Pending: %d Refill Count: %d\n",
	           cavium_atomic_read(&droq->pkts_pending),
	           droq->refill_count);
	filled_size = cvm_copy_cond(buf, str, filled_size, fill, bufsize);

	fill = sprintf(str, "Index  Read: %d Refill: %d\n",
	           droq->host_read_index, droq->host_refill_index);
	filled_size = cvm_copy_cond(buf, str, filled_size, fill, bufsize);

	fill = sprintf(str, "Register PktsSent: %d PktsCredit: 0x%08x\n",
	           OCTEON_READ32(droq->pkts_sent_reg),
	           OCTEON_READ32(droq->pkts_credit_reg));
	filled_size = cvm_copy_cond(buf, str, filled_size, fill, bufsize);

	return filled_size;
}





static int
octeon_read_status_buf(octeon_device_t  *oct,
                       char             *buf,
                       int               bufsize)
{
	int  i, len = 0;

	proc_print(buf, len, "\nChip Id: 0x%x  Status: %d (%s)\n", oct->chip_id,
	           cavium_atomic_read(&oct->status),
	           get_oct_state_string(&oct->status));

	if(oct->chip_id <= OCTEON_CN58XX)
		proc_print(buf, len, "Interrupt Mask: 0x%08x  ",
			cavium_atomic_read(&CHIP_FIELD(oct, cn3xxx, intr_mask)));


	if(oct->chip_id == OCTEON_CN56XX)
		proc_print(buf, len, "Interrupt Mask: 0x%016llx  ",
				CVM_CAST64(CHIP_FIELD(oct, cn56xx, intr_mask64)));


	if(oct->chip_id == OCTEON_CN63XX)
		proc_print(buf, len, "Interrupt Mask: 0x%016llx  ",
				CVM_CAST64(CHIP_FIELD(oct, cn63xx, intr_mask64)));

	if(oct->chip_id == OCTEON_CN66XX)
		proc_print(buf, len, "Interrupt Mask: 0x%016llx  ",
				CVM_CAST64(CHIP_FIELD(oct, cn66xx, intr_mask64)));

	

	if(oct->chip_id == OCTEON_CN68XX)
		proc_print(buf, len, "Interrupt Mask: 0x%016llx  ",
				CVM_CAST64(CHIP_FIELD(oct, cn68xx, intr_mask64)));


	proc_print(buf, len, "Interrupts: %u\n", cavium_atomic_read(&oct->interrupts));

#ifdef CAVIUM_DEBUG
	proc_print(buf, len, "Debug Level: %d\n", octeon_debug_level);
#endif

	proc_print(buf, len, "\nPending List Size: %d free_idx: %d pending: %d\n",
	           oct->pend_list_size, oct->plist->free_index,
	           cavium_atomic_read(&oct->plist->instr_count) );

	proc_print(buf, len, "Input Queues: %d Output Queues: %d\n\n",
	           oct->num_iqs, oct->num_oqs);
	for(i = 0; i < oct->num_iqs; i++) {
		len += fill_iq_status(oct, i, (buf + len), (bufsize - len));
	}

	for(i = 0; i < oct->num_oqs; i++) {
		len += fill_droq_status(oct, i, (buf + len), (bufsize - len));
	}

	return len;
}






static int
octeon_read_csrreg_buf(octeon_device_t  *oct,
                       char             *buf,
                       int               bufsize)
{

	switch(oct->chip_id) {
		case OCTEON_CN68XX:
			return cn68xx_read_csrreg_buf(oct, buf, bufsize);
		
		case OCTEON_CN66XX:
		case OCTEON_CN63XX:
			return cn63xx_read_csrreg_buf(oct, buf, bufsize);

		case OCTEON_CN56XX:
			return cn56xx_read_csrreg_buf(oct, buf, bufsize);

		case OCTEON_CN38XX_PASS2:
		case OCTEON_CN38XX_PASS3:
		case OCTEON_CN58XX:
			return cn3xxx_read_csrreg_buf(oct, buf, bufsize);
	}

	return 0;
}





static int
octeon_read_npireg_buf(octeon_device_t  *oct,
                       char             *buf,
                       int               bufsize)
{
	return cn3xxx_read_npireg_buf(oct, buf, bufsize);
}





static int
octeon_read_configreg_buf(octeon_device_t  *oct,
                          char             *buf,
                          int               bufsize)
{

	switch(oct->chip_id) {
		case OCTEON_CN68XX:
			return cn68xx_read_configreg_buf(oct, buf, bufsize);

		case OCTEON_CN66XX:
		case OCTEON_CN63XX:
			return cn63xx_read_configreg_buf(oct, buf, bufsize);

		case OCTEON_CN56XX:
			return cn56xx_read_configreg_buf(oct, buf, bufsize);

		case OCTEON_CN38XX_PASS2:
		case OCTEON_CN38XX_PASS3:
		case OCTEON_CN58XX:
			return cn3xxx_read_configreg_buf(oct, buf, bufsize);
	}

	return 0;
}



#define  PROC_STATUS_READ   1
#define  PROC_CONFIG_READ   2
#define  PROC_CSR_READ      3
#define  PROC_NPI_READ      4





/* The mechanism to read config, NPI and CSR registers and to
	get Driver status is the same. Based on the operation, we
	select the appropriate temp buffers and function to call.
	Params:
		oct - a proc file for this octeon device is being read.
		page, start, off, count, eof - all passed by the proc file
		       system to driver.
		type - internal type used by driver to select one of
		       several operations that use the same mechanism.
*/

static int
octeon_proc_readreg_buf(octeon_device_t   *oct,
                        char              *page,
                        char             **start,
                        off_t              off,
                        int                count,
                        int               *eof,
                        int                type)
{
	int   (*proc_fn)(octeon_device_t  *, char *, int);
	int   len = 0, *buflen;
	char *buf = NULL;


	switch(type) {
		case PROC_STATUS_READ:
			buf     = proc_buf[oct->octeon_id]->statusbuf;
			buflen  = &proc_buf[oct->octeon_id]->statusbuflen;
			proc_fn = octeon_read_status_buf;
			break;
		case PROC_CONFIG_READ:
			buf     = proc_buf[oct->octeon_id]->configbuf;
			buflen  = &proc_buf[oct->octeon_id]->configbuflen;
			proc_fn = octeon_read_configreg_buf;
			break;
		case PROC_CSR_READ:
			buf     = proc_buf[oct->octeon_id]->csrbuf;
			buflen  = &proc_buf[oct->octeon_id]->csrbuflen;
			proc_fn = octeon_read_csrreg_buf;
			break;
		case PROC_NPI_READ:
			buf     = proc_buf[oct->octeon_id]->npibuf;
			buflen  = &proc_buf[oct->octeon_id]->npibuflen;
			proc_fn = octeon_read_npireg_buf;
			break;
		default:
			return 0;
	}



	/* offset is 0 when the proc file is read the first time. Call the
	   corressponding routine to fill in the temporary data buffer.
	   As the system continues to read the proc file, it calls this
	   routine with the last offset into data. We keep returning data
	   as long as the temporary data buffer has more data.
	*/
	if(off == 0) {
		*buflen = proc_fn(oct, buf, REGBUFSIZE);
		len = CVM_MIN(count, *buflen);
	} else  {
		len = CVM_MIN((*buflen - off), count);
	}

	if(len) {
		memcpy(page, (buf + off), len);
		*start = page;
	}

	/* Set eof if no more bytes to be returned. */
	*eof = (len == 0);

	return len;
}








static int
proc_read_status(char *page, char **start, off_t off, int count,
                 int *eof, void *data)
{
	int len;
	octeon_device_t   *oct = (octeon_device_t *)data;

	CVM_MOD_INC_USE_COUNT;
	len = octeon_proc_readreg_buf(oct, page, start, off, count, eof,
	                             PROC_STATUS_READ);
	CVM_MOD_DEC_USE_COUNT;

	return len;
}







static int
proc_read_configregs(char *page, char **start, off_t off, int count,
                     int *eof, void *data)
{
	int len;
	octeon_device_t   *oct = (octeon_device_t *)data;

	CVM_MOD_INC_USE_COUNT;
	len = octeon_proc_readreg_buf(oct, page, start, off, count, eof,
	                             PROC_CONFIG_READ);
	CVM_MOD_DEC_USE_COUNT;

	return len;
}







static int
proc_read_csrregs(char *page, char **start, off_t off, int count,
                  int *eof, void *data)
{
	int len;
	octeon_device_t   *oct = (octeon_device_t *)data;

	CVM_MOD_INC_USE_COUNT;
	len = octeon_proc_readreg_buf(oct, page, start, off, count, eof,
	                             PROC_CSR_READ);
	CVM_MOD_DEC_USE_COUNT;

	return len;
}



static int
proc_read_npiregs(char *page, char **start, off_t off, int count,
                  int *eof, void *data)
{
	int len;
	octeon_device_t   *oct = (octeon_device_t *)data;

	CVM_MOD_INC_USE_COUNT;
	len = octeon_proc_readreg_buf(oct, page, start, off, count, eof,
	                             PROC_NPI_READ);
	CVM_MOD_DEC_USE_COUNT;

	return len;
}

















int
octeon_add_proc_entry(int  oct_id, octeon_proc_entry_t  *entry)
{
	octeon_device_t         *octeon_dev = get_octeon_device(oct_id);
	struct proc_dir_entry   *proc_entry;

 
	if(octeon_dev == NULL) {
		cavium_error("OCTEON: %s: Invalid Octeon id %d \n", __CVM_FUNCTION__,
		             oct_id);
		return ENODEV;
	}

	proc_entry = create_proc_entry(entry->name, entry->attributes,
	                               octeon_dev->proc_root_dir);
	if(proc_entry == NULL) {
		cavium_error("OCTEON: Failed to add proc entry for %s\n", entry->name);
		return ENOMEM;
	}
	SET_PROC_OWNER(proc_entry);
	
	proc_entry->data      = octeon_dev;
	if(entry->type & OCTEON_PROC_READ) {
		proc_entry->read_proc = entry->read_proc;
	}
	if(entry->type & OCTEON_PROC_WRITE) {
		proc_entry->write_proc = entry->write_proc;
	}

	return 0;
}











int
octeon_delete_proc_entry(int oct_id, char *name)
{
	octeon_device_t   *octeon_dev = get_octeon_device(oct_id);

	if(octeon_dev == NULL) {
		cavium_error("OCTEON: Invalid Octeon id %d in delete_proc_entry\n",
		             oct_id);
		return ENODEV;
	}
	remove_proc_entry(name, octeon_dev->proc_root_dir);
	return 0;
}













int 
cavium_init_proc(octeon_device_t    *octeon_dev)
{
	int                       retval = -ENOMEM;
	struct proc_dir_entry    *root, *node;

	proc_buf[octeon_dev->octeon_id] = NULL;

	/* create directory /proc/OcteonX */
	root = proc_mkdir(octeon_dev->device_name, NULL);
	if(root == NULL)
		goto proc_dir_fail;
	SET_PROC_OWNER(root);
	octeon_dev->proc_root_dir = (void *)root;


#ifdef CAVIUM_DEBUG
	/* create debug_level */
	node = create_proc_entry("debug_level", 0644, root);
	if(node == NULL)
		goto proc_fail;
	node->read_proc = proc_read_debug_level;
	node->write_proc = proc_write_debug_level;
	SET_PROC_OWNER(node);
#endif

	node = create_proc_read_entry("stats",0444, root, proc_read_stats,
	                              octeon_dev);
	if(node == NULL)
		goto proc_fail;
	SET_PROC_OWNER(node);


	node = create_proc_read_entry("configreg",0444, root, proc_read_configregs,
	                              octeon_dev);
	if(node == NULL)
		goto proc_fail;
	SET_PROC_OWNER(node);

	node = create_proc_read_entry("csrreg",0444, root, proc_read_csrregs,
	                              octeon_dev);
	if(node == NULL)
		goto proc_fail;
	SET_PROC_OWNER(node);

	if(octeon_dev->chip_id <= OCTEON_CN58XX) {
		node = create_proc_read_entry("npireg",0444, root, proc_read_npiregs,
		                              octeon_dev);
		if(node == NULL)
			goto proc_fail;
		SET_PROC_OWNER(node);
	}

	node = create_proc_read_entry("status",0444, root, proc_read_status,
	                              octeon_dev);
	if(node == NULL)
		goto proc_fail;
	SET_PROC_OWNER(node);

	proc_buf[octeon_dev->octeon_id] = 
	                  cavium_alloc_virt(sizeof(struct oct_proc_buf));
	if(proc_buf == NULL)
		goto proc_fail;

	memset(proc_buf[octeon_dev->octeon_id], 0, sizeof(struct oct_proc_buf));

	return 0;


proc_fail:
	cavium_delete_proc(octeon_dev);
proc_dir_fail:
	return retval;
}












void 
cavium_delete_proc(octeon_device_t  *oct)
{
	struct proc_dir_entry  *root = (struct proc_dir_entry *)oct->proc_root_dir;
	if(!root)
		return;

	remove_proc_entry("status", root);

	if(oct->chip_id <= OCTEON_CN58XX)
		remove_proc_entry("npireg", root);

	remove_proc_entry("csrreg", root);
	remove_proc_entry("configreg", root);
	remove_proc_entry("stats", root);
#ifdef CAVIUM_DEBUG
	remove_proc_entry("debug_level", root);
#endif
	remove_proc_entry(oct->device_name, NULL);

	if(proc_buf[oct->octeon_id])
		cavium_free_virt(proc_buf[oct->octeon_id]);

	return;
}









int
cn3xxx_read_csrreg_buf(octeon_device_t  *oct,
                       char             *buf,
                       int               bufsize)
{
	int    i, filled_size=0, fill=0;
	char  *head, *tmpbuf;

	tmpbuf = cavium_alloc_virt(bufsize);
	if(tmpbuf == NULL)
		return 0;



	/* PCI  Window Registers */
	fill = 0; head = tmpbuf;
	fill += sprintf((head + fill), "Octeon %s BAR0 Registers\n\n",
                    (oct->chip_id == OCTEON_CN58XX)?"CN58XX":"CN38XX");
	fill += sprintf((head + fill),"\n[%02x] (WIN_WR_ADDR_LO): %08x\n",
			CN3XXX_WIN_WR_ADDR_LO, octeon_read_csr(oct, CN3XXX_WIN_WR_ADDR_LO));
	fill += sprintf((head + fill), "[%02x] (WIN_WR_ADDR_HI): %08x\n",
			CN3XXX_WIN_WR_ADDR_HI, octeon_read_csr(oct, CN3XXX_WIN_WR_ADDR_HI));
	fill += sprintf((head + fill), "[%02x] (WIN_RD_ADDR_LO): %08x\n",
			CN3XXX_WIN_RD_ADDR_LO, octeon_read_csr(oct, CN3XXX_WIN_RD_ADDR_LO));
	fill += sprintf((head + fill), "[%02x] (WIN_RD_ADDR_HI): %08x\n",
			CN3XXX_WIN_RD_ADDR_HI, octeon_read_csr(oct, CN3XXX_WIN_RD_ADDR_HI));
	fill += sprintf((head + fill), "[%02x] (WIN_WR_DATA_LO): %08x\n",
			CN3XXX_WIN_WR_DATA_LO, octeon_read_csr(oct, CN3XXX_WIN_WR_DATA_LO));
	fill += sprintf((head + fill), "[%02x] (WIN_WR_DATA_HI): %08x\n",
			CN3XXX_WIN_WR_DATA_HI, octeon_read_csr(oct, CN3XXX_WIN_WR_DATA_HI));
	fill += sprintf((head + fill), "[%02x] (WIN_RD_DATA_LO): %08x\n",
			CN3XXX_WIN_RD_DATA_LO, octeon_read_csr(oct, CN3XXX_WIN_RD_DATA_LO));
	fill += sprintf((head + fill), "[%02x] (WIN_RD_DATA_HI): %08x\n",
			CN3XXX_WIN_RD_DATA_HI, octeon_read_csr(oct, CN3XXX_WIN_RD_DATA_HI));
	fill += sprintf((head + fill), "[%02x] (WIN_WR_MASK_REG): %08x\n",
		CN3XXX_WIN_WR_MASK_REG, octeon_read_csr(oct, CN3XXX_WIN_WR_MASK_REG));

	filled_size = cvm_copy_cond(buf, tmpbuf, filled_size, fill, bufsize);

	/* PCI  Interrupt Register */
	fill = 0; head = tmpbuf;
	fill += sprintf((head + fill), "\n[%x] (INT_ENABLE): %08x\n",
				PCI_INT_ENABLE, octeon_read_csr(oct, PCI_INT_ENABLE));
	fill += sprintf((head + fill), "[%x] (INT_SUM): %08x\n",
				PCI_INT_SUM, octeon_read_csr(oct, PCI_INT_SUM));

	filled_size = cvm_copy_cond(buf, tmpbuf, filled_size, fill, bufsize);


	fill = 0; head = tmpbuf;

	/* PCI  Output queue registers */
	for(i = 0; i < oct->num_oqs; i++) {
		uint32_t   reg;

		reg = OCT_OQ_PKTS_SENT_REG(i);
		fill += sprintf((head + fill), "\n[%x] (PKTS_SENT_%d): %08x\n",
		                 reg, i, octeon_read_csr(oct, reg));
		reg = OCT_OQ_PKTS_CREDITS_REG(i);
		fill += sprintf((head + fill), "[%x] (PKT_CREDITS_%d): %08x\n",
		                 reg, i, octeon_read_csr(oct, reg));
		reg = OCT_OQ_INTR_COUNT_LEV_REG(i);

		fill +=sprintf((head+fill),"[%x] (PKTS_SENT_INT_LEV_%d): %08x\n",
		                 reg, i, octeon_read_csr(oct, reg));
		reg = OCT_OQ_INTR_TIME_LEV_REG(i);
		fill +=sprintf((head + fill), "[%x] (PKTS_SENT_TIME_%d): %08x\n",
		                 reg, i, octeon_read_csr(oct, reg));
	}

	filled_size = cvm_copy_cond(buf, tmpbuf, filled_size, fill, bufsize);


	fill = 0; head = tmpbuf;

	/* PCI  Input queue registers */
	for(i = 0; i < oct->num_iqs; i++) {
		uint32_t   reg;

		reg = OCT_IQ_DOORBELL_REG(i);
		fill += sprintf((head + fill),
						"\n[%x] (INSTR_DOORBELL_%d): %08x\n",
				 		reg, i, octeon_read_csr(oct,  reg));
		reg = OCT_IQ_COUNT_REG(i);
		fill += sprintf((head+fill),"[%x] (INSTR_COUNT_%d): %08x\n",
				 		reg, i, octeon_read_csr(oct,  reg));
	}

	filled_size = cvm_copy_cond(buf, tmpbuf, filled_size, fill, bufsize);


	/* PCI  DMA registers */
	fill = 0; head = tmpbuf;

	fill += sprintf((head + fill), "\n[%x] (DMA_CNT_0): %08x\n",
			CN3XXX_DMA_CNT(0), octeon_read_csr(oct, CN3XXX_DMA_CNT(0)));
	fill += sprintf((head + fill), "[%x] (DMA_INT_LEV_0): %08x\n",
			CN3XXX_DMA_PKT_INT_LEVEL(0),
			octeon_read_csr(oct, CN3XXX_DMA_PKT_INT_LEVEL(0)));
	fill += sprintf((head + fill), "[%x] (DMA_TIME_0): %08x\n",
			CN3XXX_DMA_TIME_INT_LEVEL(0),
			octeon_read_csr(oct, CN3XXX_DMA_TIME_INT_LEVEL(0)));

	filled_size = cvm_copy_cond(buf, tmpbuf, filled_size, fill, bufsize);

	fill = 0; head = tmpbuf;

	fill += sprintf((head + fill), "\n[%x] (DMA_CNT_1): %08x\n",
			CN3XXX_DMA_CNT(1), octeon_read_csr(oct, CN3XXX_DMA_CNT(1)));
	fill += sprintf((head + fill), "[%x] (DMA_INT_LEV_1): %08x\n",
			CN3XXX_DMA_PKT_INT_LEVEL(1),
			octeon_read_csr(oct, CN3XXX_DMA_PKT_INT_LEVEL(1)));
	fill += sprintf((head + fill), "[%x] (DMA_TIME_1): %08x\n",
			CN3XXX_DMA_TIME_INT_LEVEL(1),
			octeon_read_csr(oct, CN3XXX_DMA_TIME_INT_LEVEL(1)));

	filled_size = cvm_copy_cond(buf, tmpbuf, filled_size, fill, bufsize);

	/* PCI  Index registers */
	fill = 0; head = tmpbuf;

	fill += sprintf((head + fill), "\n");
	for(i = 0; i < 32; i++) {
		uint32_t   reg = OCT_BAR1_INDEX_REG(i);
		fill +=sprintf((head+fill),"[%x] (BAR1_INDEX_%02d): %08x\n",
				 		reg, i, octeon_read_csr(oct,  reg));
	}

	filled_size = cvm_copy_cond(buf, tmpbuf, filled_size, fill, bufsize);

	cavium_free_virt(tmpbuf);

	return filled_size;
}











int
cn3xxx_read_npireg_buf(octeon_device_t  *oct,
                       char             *buf,
                       int               bufsize)
{
	int       i, filled_size=0, fill=0;
	uint64_t  reg;
	char     *head, *tmpbuf;

	tmpbuf = cavium_alloc_virt(bufsize);
	if(tmpbuf == NULL)
		return 0;

	fill = 0; head = tmpbuf;
	fill += sprintf((head + fill), "Octeon %s NPI Registers\n",
                    (oct->chip_id == OCTEON_CN58XX)?"CN58XX":"CN38XX");

	fill += sprintf((head + fill), "\nNPI Control Registers\n");
	reg = NPI_INPUT_CONTROL;
	fill += sprintf((head + fill),
					"[%016llx] (NPI_INPUT_CONTROL): %016llx\n", 
					CVM_CAST64(reg),
					CVM_CAST64(OCTEON_PCI_WIN_READ(oct, reg)));
	reg = NPI_OUTPUT_CONTROL;
	fill += sprintf((head + fill),
					"[%016llx] (NPI_OUTPUT_CONTROL): %016llx\n", 
					CVM_CAST64(reg),
					CVM_CAST64(OCTEON_PCI_WIN_READ(oct, reg)));
	reg = NPI_CTL_STATUS;
	fill += sprintf((head + fill),
					"[%016llx] (NPI_CTL_STATUS): %016llx\n", 
					CVM_CAST64(reg),
					CVM_CAST64(OCTEON_PCI_WIN_READ(oct, reg)));
	reg = NPI_PCI_BURST_SIZE;
	fill += sprintf((head + fill),
					"[%016llx] (NPI_PCI_BURST_SIZE): %016llx\n", 
					CVM_CAST64(reg),
					CVM_CAST64(OCTEON_PCI_WIN_READ(oct, reg)));
	reg = NPI_PCI_READ_TIMEOUT;
	fill += sprintf((head + fill),
					"[%016llx] (NPI_PCI_READ_TIMEOUT): %016llx\n", 
					CVM_CAST64(reg),
					CVM_CAST64(OCTEON_PCI_WIN_READ(oct, reg)));

	filled_size = cvm_copy_cond(buf, tmpbuf, filled_size, fill, bufsize);


	fill = 0; head = tmpbuf;
	fill += sprintf((head + fill), "\nNPI Input Queue Registers\n");
	for(i = 0; i < oct->num_iqs; i++) {
		reg = OCT_IQ_BASE_ADDR_REG(i);
		fill += sprintf((head + fill),
						"[%016llx] (BASE_ADDR_INPUT_%d): %016llx\n", 
						CVM_CAST64(reg), i,
						CVM_CAST64(OCTEON_PCI_WIN_READ(oct, reg)));
		reg = OCT_IQ_SIZE_REG(i);
		fill += sprintf((head + fill),
						"[%016llx] (BASE_SIZE_INPUT_%d): %016llx\n", 
						CVM_CAST64(reg), i,
						CVM_CAST64(OCTEON_PCI_WIN_READ(oct, reg)));
		reg = NPI_INSTR_CNTS_DEBUG_BASE + (i * NPI_INPUT_DEBUG_OFFSET);
		fill += sprintf((head + fill),
						"[%016llx] (P%d_INSTR_CNTS): %016llx\n", 
						CVM_CAST64(reg), i,
						CVM_CAST64(OCTEON_PCI_WIN_READ(oct, reg)));
		reg = NPI_INSTR_ADDR_DEBUG_BASE + (i * NPI_INPUT_DEBUG_OFFSET);
		fill += sprintf((head + fill),
						"[%016llx] (P%d_INSTR_ADDR): %016llx\n", 
						CVM_CAST64(reg), i,
						CVM_CAST64(OCTEON_PCI_WIN_READ(oct, reg)));
		reg = OCT_IQ_INSTR_HDR_REG(i);
		fill += sprintf((head + fill),
						"[%016llx] (PORT%d_INSTR_HDR): %016llx\n", 
						CVM_CAST64(reg), 32+i,
						CVM_CAST64(OCTEON_PCI_WIN_READ(oct, reg)));
		fill += sprintf((head + fill),"\n");
	}

	filled_size = cvm_copy_cond(buf, tmpbuf, filled_size, fill, bufsize);



	fill = 0; head = tmpbuf;
	fill += sprintf((head + fill), "\nNPI Output Queue Registers\n");
	for(i = 0; i < oct->num_oqs; i++) {
		reg = OCT_OQ_BASE_ADDR_REG(i);
		fill += sprintf((head + fill),
						"[%016llx] (BASE_ADDR_OUTPUT_%d): %016llx\n", 
						CVM_CAST64(reg), i,
						CVM_CAST64(OCTEON_PCI_WIN_READ(oct, reg)));
		reg = OCT_OQ_BUFF_SIZE_REG(i);
		fill += sprintf((head + fill),
						"[%016llx] (BUFF_SIZE_OUTPUT_%d): %016llx\n", 
						CVM_CAST64(reg), i,
						CVM_CAST64(OCTEON_PCI_WIN_READ(oct, reg)));
		reg = OCT_OQ_SIZE_REG(i);
		fill += sprintf((head + fill),
						"[%016llx] (NUM_DESC_OUTPUT_%d): %016llx\n", 
						CVM_CAST64(reg), i,
						CVM_CAST64(OCTEON_PCI_WIN_READ(oct, reg)));
		reg = NPI_PAIR_CNTS_DEBUG_BASE + (i * NPI_OQ_OFFSET);
		fill += sprintf((head + fill),
						"[%016llx] (P%d_PAIR_CNTS): %016llx\n", 
						CVM_CAST64(reg), i,
						CVM_CAST64(OCTEON_PCI_WIN_READ(oct, reg)));
		reg = NPI_DBPAIR_ADDR_DEBUG_BASE + (i * NPI_OQ_OFFSET);
		fill += sprintf((head + fill),
						"[%016llx] (P%d_DBPAIR_ADDR): %016llx\n", 
						CVM_CAST64(reg), i,
						CVM_CAST64(OCTEON_PCI_WIN_READ(oct, reg)));
		fill += sprintf((head + fill),"\n");
	}

	filled_size = cvm_copy_cond(buf, tmpbuf, filled_size, fill, bufsize);

	cavium_free_virt(tmpbuf);

	return filled_size;
}













int
cn3xxx_read_configreg_buf(octeon_device_t  *oct,
                          char             *buf,
                          int               bufsize)
{
	int       i, filled_size=0, fill=0;
	uint32_t  val;
	char     *head, *tmpbuf;

	tmpbuf = cavium_alloc_virt(bufsize);
	if(tmpbuf == NULL)
		return 0;


	/* PCI  CONFIG Registers */
	fill = 0; head = tmpbuf;

	fill += sprintf((head + fill), "Octeon %s PCI Config space Registers\n\n",
	                (oct->chip_id == OCTEON_CN58XX)?"CN58XX":"CN38XX");
	filled_size = cvm_copy_cond(buf, tmpbuf, filled_size, fill, bufsize);

	fill = 0; head = tmpbuf;
	for(i = 0; i <= 22; i++)  {
		OCTEON_READ_PCI_CONFIG(oct, (i*4), &val);
		fill += sprintf((head + fill), "[0x%x] (Config[%d]): 0x%08x\n",
			(i*4), i, val);
	}
	filled_size = cvm_copy_cond(buf, tmpbuf, filled_size, fill, bufsize);


	fill = 0; head = tmpbuf;
	OCTEON_READ_PCI_CONFIG(oct, 0xE0, &val);
	fill += sprintf((head + fill), "[0xE0] (Config[56]): 0x%08x\n", val);
	filled_size = cvm_copy_cond(buf, tmpbuf, filled_size, fill, bufsize);

	cavium_free_virt(tmpbuf);

	return filled_size;
}







int
cn56xx_read_csrreg_buf(octeon_device_t  *oct,
                       char             *buf,
                       int               bufsize)
{
	int        i, filled_size=0, fill=0;
	char      *head, *tmpbuf;
	uint32_t   reg;


	tmpbuf = cavium_alloc_virt(bufsize);
	if(tmpbuf == NULL)
		return 0;

	/* PCI  Window Registers */
	fill = 0; head = tmpbuf;

	fill += sprintf((head + fill), "Octeon CN56XX BAR0 Registers\n\n");
	fill += sprintf((head + fill),"\n[%02x] (WIN_WR_ADDR_LO): %08x\n",
		CN56XX_WIN_WR_ADDR_LO, octeon_read_csr(oct, CN56XX_WIN_WR_ADDR_LO));
	fill += sprintf((head + fill), "[%02x] (WIN_WR_ADDR_HI): %08x\n",
		CN56XX_WIN_WR_ADDR_HI, octeon_read_csr(oct, CN56XX_WIN_WR_ADDR_HI));
	fill += sprintf((head + fill), "[%02x] (WIN_RD_ADDR_LO): %08x\n",
		CN56XX_WIN_RD_ADDR_LO, octeon_read_csr(oct, CN56XX_WIN_RD_ADDR_LO));
	fill += sprintf((head + fill), "[%02x] (WIN_RD_ADDR_HI): %08x\n",
		CN56XX_WIN_RD_ADDR_HI, octeon_read_csr(oct, CN56XX_WIN_RD_ADDR_HI));
	fill += sprintf((head + fill), "[%02x] (WIN_WR_DATA_LO): %08x\n",
		CN56XX_WIN_WR_DATA_LO, octeon_read_csr(oct, CN56XX_WIN_WR_DATA_LO));
	fill += sprintf((head + fill), "[%02x] (WIN_WR_DATA_HI): %08x\n",
		CN56XX_WIN_WR_DATA_HI, octeon_read_csr(oct, CN56XX_WIN_WR_DATA_HI));
	fill += sprintf((head + fill), "[%02x] (WIN_WR_MASK_REG): %08x\n",
		CN56XX_WIN_WR_MASK_REG, octeon_read_csr(oct, CN56XX_WIN_WR_MASK_REG));


	filled_size = cvm_copy_cond(buf, tmpbuf, filled_size, fill, bufsize);

	/* PCI  Interrupt Register */
	fill = 0; head = tmpbuf;
	fill += sprintf((head + fill), "\n[%x] (INT_ENABLE): %08x\n",
				CN56XX_NPEI_INT_ENB64, octeon_read_csr(oct, CN56XX_NPEI_INT_ENB64));
	fill += sprintf((head + fill), "[%x] (INT_SUM): %08x\n",
				CN56XX_NPEI_INT_SUM64, octeon_read_csr(oct, CN56XX_NPEI_INT_SUM64));

	filled_size = cvm_copy_cond(buf, tmpbuf, filled_size, fill, bufsize);


	fill = 0; head = tmpbuf;

	/* PCI  Output queue registers */
	for(i = 0; i < oct->num_oqs; i++) {

		reg = CN56XX_NPEI_OQ_PKTS_SENT(i);
		fill += sprintf((head + fill), "\n[%x] (PKTS_SENT_%d): %08x\n",
		                 reg, i, octeon_read_csr(oct, reg));
		reg = CN56XX_NPEI_OQ_PKTS_CREDIT(i);
		fill += sprintf((head + fill), "[%x] (PKT_CREDITS_%d): %08x\n",
		                 reg, i, octeon_read_csr(oct, reg));
	}

	reg = CN56XX_NPEI_OQ_INT_LEV_PKTS;
	fill +=sprintf((head+fill),"\n[%x] (PKTS_SENT_INT_LEV): %08x\n",
	                 reg, octeon_read_csr(oct, reg));
	reg = CN56XX_NPEI_OQ_INT_LEV_TIME;
	fill +=sprintf((head + fill), "[%x] (PKTS_SENT_TIME): %08x\n",
	                 reg, octeon_read_csr(oct, reg));

	filled_size = cvm_copy_cond(buf, tmpbuf, filled_size, fill, bufsize);



	fill = 0; head = tmpbuf;

	/* PCI  Input queue registers */
	for(i = 0; i <= 3; i++) {
		uint32_t   reg;

		reg = CN56XX_NPEI_IQ_DOORBELL(i);
		fill += sprintf((head + fill),
						"\n[%x] (INSTR_DOORBELL_%d): %08x\n",
				 		reg, i, octeon_read_csr(oct,  reg));
		reg = CN56XX_NPEI_IQ_INSTR_COUNT(i);
		fill += sprintf((head+fill),"[%x] (INSTR_COUNT_%d): %08x\n",
				 		reg, i, octeon_read_csr(oct,  reg));
	}

	filled_size = cvm_copy_cond(buf, tmpbuf, filled_size, fill, bufsize);


	/* PCI  DMA registers */
	fill = 0; head = tmpbuf;

	fill += sprintf((head + fill), "\n[%x] (DMA_CNT_0): %08x\n",
			CN56XX_DMA_CNT(0), octeon_read_csr(oct, CN56XX_DMA_CNT(0)));
	fill += sprintf((head + fill), "[%x] (DMA_INT_LEV_0): %08x\n",
			CN56XX_DMA_PKT_INT_LEVEL(0),
			octeon_read_csr(oct, CN56XX_DMA_PKT_INT_LEVEL(0)));
	fill += sprintf((head + fill), "[%x] (DMA_TIME_0): %08x\n",
			CN56XX_DMA_TIME_INT_LEVEL(0),
			octeon_read_csr(oct, CN56XX_DMA_TIME_INT_LEVEL(0)));

	filled_size = cvm_copy_cond(buf, tmpbuf, filled_size, fill, bufsize);


	fill = 0; head = tmpbuf;
	fill += sprintf((head + fill), "\n[%x] (DMA_CNT_1): %08x\n",
			CN56XX_DMA_CNT(1), octeon_read_csr(oct, CN56XX_DMA_CNT(1)));
	fill += sprintf((head + fill), "[%x] (DMA_INT_LEV_1): %08x\n",
			CN56XX_DMA_PKT_INT_LEVEL(1),
			octeon_read_csr(oct, CN56XX_DMA_PKT_INT_LEVEL(1)));
	fill += sprintf((head + fill), "[%x] (DMA_TIME_1): %08x\n",
			CN56XX_DMA_TIME_INT_LEVEL(1),
			octeon_read_csr(oct, CN56XX_DMA_TIME_INT_LEVEL(1)));

	filled_size = cvm_copy_cond(buf, tmpbuf, filled_size, fill, bufsize);

	/* PCI  Index registers */
	fill = 0; head = tmpbuf;

	fill += sprintf((head + fill), "\n");

	for(i = 0; i < 32; i++) {
		reg = CN56XX_BAR1_INDEX_REG(i);
		fill +=sprintf((head+fill),"[%x] (BAR1_INDEX_%02d): %08x\n",
				 		reg, i, octeon_read_csr(oct,  reg));
	}

	filled_size = cvm_copy_cond(buf, tmpbuf, filled_size, fill, bufsize);

	cavium_free_virt(tmpbuf);

	return filled_size;
}








int
cn56xx_read_configreg_buf(octeon_device_t  *oct,
                          char             *buf,
                          int               bufsize)
{
	int        i, filled_size=0, fill=0;
	uint32_t   val;
	char      *head, *tmpbuf;

	tmpbuf = cavium_alloc_virt(bufsize);
	if(tmpbuf == NULL)
		return 0;


	/* PCI CONFIG Registers */
	fill = 0; head = tmpbuf;

	fill += sprintf((head + fill),
	                "Octeon CN56XX PCI Config space Registers\n\n");
	filled_size = cvm_copy_cond(buf, tmpbuf, filled_size, fill, bufsize);

	fill = 0; head = tmpbuf;
	for(i = 0; i <= 13; i++)  {
		OCTEON_READ_PCI_CONFIG(oct, (i*4), &val);
		fill += sprintf((head + fill), "[0x%x] (Config[%d]): 0x%08x\n",
			(i*4), i, val);
	}
	filled_size = cvm_copy_cond(buf, tmpbuf, filled_size, fill, bufsize);

	fill = 0; head = tmpbuf;
	for(i = 30; i <= 34; i++)  {
		OCTEON_READ_PCI_CONFIG(oct, (i*4), &val);
		fill += sprintf((head + fill), "[0x%x] (Config[%d]): 0x%08x\n",
			(i*4), i, val);
	}
	filled_size = cvm_copy_cond(buf, tmpbuf, filled_size, fill, bufsize);

	cavium_free_virt(tmpbuf);

	return filled_size;
}




int
cn63xx_read_configreg_buf(octeon_device_t  *oct,
                          char             *buf,
                          int               bufsize)
{
	return 0;
}



int
cn63xx_read_csrreg_buf(octeon_device_t  *oct,
                       char             *buf,
                       int               bufsize)
{
	return 0;
}




int
cn68xx_read_configreg_buf(octeon_device_t  *oct,
                          char             *buf,
                          int               bufsize)
{
	return 0;
}

int
cn68xx_read_csrreg_buf(octeon_device_t  *oct,
                       char             *buf,
                       int               bufsize)
{
	return 0;
}

/* $Id: cavium_proc.c 66446 2011-10-25 02:31:59Z mchalla $ */
