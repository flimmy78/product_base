#ifndef __NPD_COMMUNICATION_H_
#define __NPD_COMMUNICATION_H_

#define NPD_COMM_TYPE_SLOT_ZERO_LOW       1200
#define NPD_COMM_TYPE_SLOT_ZERO_UP        1200
#define NPD_COMM_TYPE_SLOT_ZERO_INSTANCE  1200

#define NPD_COMM_TYPE_SLOT_ONE_LOW        1250
#define NPD_COMM_TYPE_SLOT_ONE_UP         1250
#define NPD_COMM_TYPE_SLOT_ONE_INSTANCE   1250

#define NPD_COMM_TYPE_SLOT_TWO_LOW        1300
#define NPD_COMM_TYPE_SLOT_TWO_UP         1300
#define NPD_COMM_TYPE_SLOT_TWO_INSTANCE   1300

#define NPD_TIPC_RECV_BUF_LEN   512

#define NPD_SERVER_STAT 100
#define NPD_CLIETN_STAT 200

int gServerTypeLow;
int gServerTypeUP;
int gClientInstance;
int gNpdStat;

typedef struct
{
        int thread_index;
        int sd;
}thread_index_sd_t;

typedef struct
{
    int slot;
    int opcode;
}npd_cmd_t;

extern int  npd_communication_init();
extern void npd_creat_server(void* arg);
extern void npd_creat_client(void* arg);
int npd_send(char *send_buf, int send_buf_len, char *recv_buf, int *recv_buf_len);


#endif
