#ifndef _HMD_MONITOR_H
#define _HMD_MONITOR_H
void * HMDHansiMonitor(void *arg);
unsigned int notice_wid_local_hansi_service_change_state(unsigned int InstID, unsigned int neighbor_slotid);
int send_tunnel_interface_arp(char *MAC, int addr, char* ifname );
unsigned int notice_eag_local_hansi_service_change_state(unsigned int InstID, unsigned int neighbor_slotid);
void HMDReInitHadDbusPath(int index, char * path, char * newpath, int islocaled);
#endif

