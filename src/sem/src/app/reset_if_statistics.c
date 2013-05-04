#include <stdio.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdlib.h>

typedef struct sta_reset {
	char dev_name[32];
	unsigned int param_count;
	struct name_value {
		char name[32];
		unsigned long long value; 
	}name_value_arry[20];
}if_sta_statistics_str;


#define CVM_IOC_MAGIC 242
#define CVM_IOC_RESET_IF_STA	_IOWR(CVM_IOC_MAGIC, 30, if_sta_statistics_str)

/*
  *rx_packets--rx_packets
  *rx_errors--rx_errors
  *rx_dropped
  *rx_overruns--rx_fifo_errors
  *rx_fram
  *rx_bytes--rx_bytes
  *
  *tx_packets--tx_packets
  *tx_errors--tx_errors
  *tx_dropped--tx_dropped
  *tx_overruns--tx_fifo_errors
  *tx_carrier
  *tx_bytes--tx_bytes
  */
  
void print_prompt_info(void)
{
	printf("please enter parameter like this:\n");
	printf("for example:\n");
	printf("./reset_if_statistics device_name rx_packets 100 tx_packets 200 rx_dropped 300\n");
}

int main(int argc, char *argv[])
{
	int fd = 0, retval, i, j = 0;
	if_sta_statistics_str temp_str;
	unsigned long value;
	
	//printf("CVM_IOC_RESET_IF_STA=%lu sizeof(if_sta_statistics_str)=%lu         \n", 
	//CVM_IOC_RESET_IF_STA, sizeof(if_sta_statistics_str));
	//printf("sizeof(unsigned int)=%lu sizeof(unsigned char)=%lu sizeof(unsigned long long)=%lu\n",
	//	sizeof(unsigned int), sizeof(unsigned char), sizeof(unsigned long long));
	//sleep(2);

	if ((argc % 2) != 0 || argc <= 2) {
		printf("parameter is mismatch,check please.\n");
		print_prompt_info();
		return 1;
	}
	
	for (i=2; i<argc; i=i+2,j++)
	{
		if (strcmp(argv[i], "rx_packets") == 0) {
			value = strtoul(argv[i+1], NULL, 10);
			#if 0
			if (value == ERANGE) {
				printf("parameter %d erro: %s.only digit is allowed\n", i+1, argv[i+1]);
				return 1;
			} else {
				strcpy(temp_str.name_value_arry[j].name, argv[i]);
				temp_str.name_value_arry[j].value = (unsigned long long)value;
			}
			#else
			strcpy(temp_str.name_value_arry[j].name, argv[i]);
			temp_str.name_value_arry[j].value = (unsigned long long)value;
			#endif
		} else if (strcmp(argv[i], "tx_packets") == 0) {
			value = strtoul(argv[i+1], NULL, 10);
			strcpy(temp_str.name_value_arry[j].name, argv[i]);
			temp_str.name_value_arry[j].value = (unsigned long long)value;
		} else if (strcmp(argv[i], "rx_bytes") == 0) {
			value = strtoul(argv[i+1], NULL, 10);
			strcpy(temp_str.name_value_arry[j].name, argv[i]);
			temp_str.name_value_arry[j].value = (unsigned long long)value;
		} else if (strcmp(argv[i], "tx_bytes") == 0) {
			value = strtoul(argv[i+1], NULL, 10);
			strcpy(temp_str.name_value_arry[j].name, argv[i]);
			temp_str.name_value_arry[j].value = (unsigned long long)value;
		} else if (strcmp(argv[i], "rx_errors") == 0) {
			value = strtoul(argv[i+1], NULL, 10);
			strcpy(temp_str.name_value_arry[j].name, argv[i]);
			temp_str.name_value_arry[j].value = (unsigned long long)value;		
		} else if (strcmp(argv[i], "tx_errors") == 0) {
			value = strtoul(argv[i+1], NULL, 10);
			strcpy(temp_str.name_value_arry[j].name, argv[i]);
			temp_str.name_value_arry[j].value = (unsigned long long)value;		
		} else if (strcmp(argv[i], "rx_dropped") == 0) {
			value = strtoul(argv[i+1], NULL, 10);
			strcpy(temp_str.name_value_arry[j].name, argv[i]);
			temp_str.name_value_arry[j].value = (unsigned long long)value;
		} else if (strcmp(argv[i], "tx_dropped") == 0) {
			value = strtoul(argv[i+1], NULL, 10);
			strcpy(temp_str.name_value_arry[j].name, argv[i]);
			temp_str.name_value_arry[j].value = (unsigned long long)value;
		} else if (strcmp(argv[i], "rx_length_errors") == 0) {
			value = strtoul(argv[i+1], NULL, 10);
			strcpy(temp_str.name_value_arry[j].name, argv[i]);
			temp_str.name_value_arry[j].value = (unsigned long long)value;
		} else if (strcmp(argv[i], "tx_aborted_errors") == 0) {
			value = strtoul(argv[i+1], NULL, 10);
			strcpy(temp_str.name_value_arry[j].name, argv[i]);
			temp_str.name_value_arry[j].value = (unsigned long long)value;
		} else if (strcmp(argv[i], "rx_over_errors") == 0) {
			value = strtoul(argv[i+1], NULL, 10);
			strcpy(temp_str.name_value_arry[j].name, argv[i]);
			temp_str.name_value_arry[j].value = (unsigned long long)value;
		} else if (strcmp(argv[i], "tx_carrier_errors") == 0) {
			value = strtoul(argv[i+1], NULL, 10);
			strcpy(temp_str.name_value_arry[j].name, argv[i]);
			temp_str.name_value_arry[j].value = (unsigned long long)value;
		} else if (strcmp(argv[i], "rx_crc_errors") == 0) {
			value = strtoul(argv[i+1], NULL, 10);
			strcpy(temp_str.name_value_arry[j].name, argv[i]);
			temp_str.name_value_arry[j].value = (unsigned long long)value;
		} else if (strcmp(argv[i], "tx_fifo_errors") == 0) {
			value = strtoul(argv[i+1], NULL, 10);
			strcpy(temp_str.name_value_arry[j].name, argv[i]);
			temp_str.name_value_arry[j].value = (unsigned long long)value;
		} else if (strcmp(argv[i], "rx_frame_errors") == 0) {
			value = strtoul(argv[i+1], NULL, 10);
			strcpy(temp_str.name_value_arry[j].name, argv[i]);
			temp_str.name_value_arry[j].value = (unsigned long long)value;
		} else if (strcmp(argv[i], "tx_heartbeat_errors") == 0) {
			value = strtoul(argv[i+1], NULL, 10);
			strcpy(temp_str.name_value_arry[j].name, argv[i]);
			temp_str.name_value_arry[j].value = (unsigned long long)value;
		} else if (strcmp(argv[i], "rx_fifo_errors") == 0) {
			value = strtoul(argv[i+1], NULL, 10);
			strcpy(temp_str.name_value_arry[j].name, argv[i]);
			temp_str.name_value_arry[j].value = (unsigned long long)value;
		} else if (strcmp(argv[i], "tx_window_errors") == 0) {
			value = strtoul(argv[i+1], NULL, 10);
			strcpy(temp_str.name_value_arry[j].name, argv[i]);
			temp_str.name_value_arry[j].value = (unsigned long long)value;
		} else if (strcmp(argv[i], "rx_missed_errors") == 0) {
			value = strtoul(argv[i+1], NULL, 10);
			strcpy(temp_str.name_value_arry[j].name, argv[i]);
			temp_str.name_value_arry[j].value = (unsigned long long)value;
		} else if (strcmp(argv[i], "multicast") == 0) {
			value = strtoul(argv[i+1], NULL, 10);
			strcpy(temp_str.name_value_arry[j].name, argv[i]);
			temp_str.name_value_arry[j].value = (unsigned long long)value;
		} else {
			printf("parameter %d erro: %s.\n", i+1, argv[i+1]);
			print_prompt_info();
			return 1;
		}
	}

	strcpy(temp_str.dev_name, argv[1]);
	temp_str.param_count = (argc-2)/2;
		
	fd = open("/dev/oct0", 0);
	if (fd == -1)
	{
		printf("open /dev/oct0 failed\n");
		return 1;
	}
	else 
	{
#if 0
		strcpy(temp_str.dev_name, "ve03f1");
		temp_str.param_count = 5;
		
		strcpy(temp_str.name_value_arry[0].name, "rx_packets");
		temp_str.name_value_arry[0].value = 88888;
		
		strcpy(temp_str.name_value_arry[1].name, "tx_packets");
		temp_str.name_value_arry[1].value = 99999;
		
		strcpy(temp_str.name_value_arry[2].name, "rx_length_errors");
		temp_str.name_value_arry[2].value = 777777;
		
		strcpy(temp_str.name_value_arry[3].name, "tx_errors");
		temp_str.name_value_arry[3].value = 6666666;
		
		strcpy(temp_str.name_value_arry[4].name, "tx_carrier_errors");
		temp_str.name_value_arry[4].value = 55555555;
#endif		
		retval = ioctl(fd, CVM_IOC_RESET_IF_STA, &temp_str);
		if (0 == retval)
		{
			printf("set success\n");
			close(fd);
			return 0;
		}
		else
		{
			printf("set failed\n");
			close(fd);
			return 1;
		}
	}
}
