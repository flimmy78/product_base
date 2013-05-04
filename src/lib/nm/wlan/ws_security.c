/* cgicTempDir is the only setting you are likely to need
	to change in this file. */

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
* ws_security.c
*
*
* CREATOR:
* autelan.software.Network Dep. team
* qiaojie@autelan.com
*
* DESCRIPTION:
*
*
*
*******************************************************************************/


#ifdef __cplusplus
extern "C"
{
#endif

#include "ws_security.h"


int parse_security_char_ID(char* str,unsigned char* ID){
	char *endptr = NULL;
	char c;
	c = str[0];
	if (c>='0'&&c<='9'){
		*ID= strtoul(str,&endptr,10);
		if(endptr[0] == '\0')
			return WID_DBUS_SUCCESS;
		else
			return WID_UNKNOWN_ID;
	}
	else
		return WID_UNKNOWN_ID;
}


/*dcli_security.c V1.24*/
/*author qiaojie*/
/*update time 08-8-22*/



/*dcli_security.c V1.37*/
/*author liutao*/
/*update time 08-12-30*/


/*dcli_security.c V1.102*/
/*author qiaojie*/
/*update time 10-03-30*/

int RemoveListRepId(int list[],int num)

{
	int i,j,k;
	for(i=0;i<num;i++){ 
        for(j=i+1;j<num;j++)  { 
              if(list[i]==list[j])  { 
                  num--;
                  for(k=j;k<num;k++) { 
                       list[k]=list[k+1]; 
                  } 
                  j--; 
               } 
         } 
     } 
   //  for(i=0;i<num;i++){
	// printf("%d",list[i]);
   //  printf("\n");
	// }
     return num; 

}

int parse_vlan_list(char* ptr,int* count,int vlanId[])
{
	char* endPtr = NULL;
	int   i=0;
	endPtr = ptr;
	vlan_list_state state = check_vlanid_state;

	while(1){
		switch(state){
			
		case check_vlanid_state: 
			//	printf("enter vlanid\n");
				vlanId[i] = strtoul(endPtr,&endPtr,10);
			//	printf("vlanid%d  %d\n",i,vlanId[i]);
				if(vlanId[i]>0&&vlanId[i]<4095){
            		state=check_comma_state;

				
			//		printf("vlanid %d\n",vlanId[i]);
				}
				else
					state=check_fail_state;
				
				break;
		
		case check_comma_state: 
			//printf("enter comma\n");
			if (SLOT_PORT_SPLIT_COMMA== endPtr[0]){
				endPtr = (char*)endPtr + 1;
				state = check_vlanid_state;
			//	printf("%d\n",state);
				i++;
				}
			else
				state = check_end_state;
			break;
				
		
		case check_fail_state:
		//	printf("enter fail\n");
		//	printf("%d\n",state);
			return -1;
			break;

		case check_end_state: 
		//	printf("enter end\n");
			if ('\0' == endPtr[0]) {
				state = check_success_state;
				i++;
				}
			else
				state = check_fail_state;
				break;
			
		case check_success_state: 
		//	printf("enter success\n");
		//	printf("%d\n",state);
			
			*count = i;
			return 0;
			break;
			
		default: break;
		}
		
		}
		
}

int parse_port(char* ptr,/*int* slot,int* port*/SLOT_PORT_VLAN_SECURITY* sp){

	char* endPtr = NULL;
    short slot = 0,port = 0;
//    int   i=0;
	PARSE_PORT_STATE parse_state=TEST_SLOT_STATE;
	
    endPtr = ptr;

	//use a state machine to parse the port.  six states.
	while(1){
			switch(parse_state)
			{
				case TEST_SLOT_STATE:
					
					slot = strtoul(endPtr,&endPtr,10);
				//	printf("enter slot %d\n",slot);
					if(slot>0&&slot<=4){
            			sp->slot = slot;
						parse_state=TEST_SPLIT_STATE;
					}
					else
						parse_state=TEST_FAILURE_STATE;
					break;
					
				case TEST_SPLIT_STATE:
				//	printf("enter split\n");
					if (SLOT_PORT_SPLIT_SLASH== endPtr[0]||SLOT_PORT_SPLIT_DASH == endPtr[0]) 
						parse_state=TEST_PORT_STATE;
					else
						parse_state=TEST_FAILURE_STATE;
					break;
					
				case TEST_PORT_STATE:
					
					port = strtoul((char *)&(endPtr[1]),&endPtr,10);
				//	printf("enter port %d\n",port);
					if(port>0&&port<=24){
               			sp->port = port;
						parse_state=TEST_END_STATE;
					}
           			else
						parse_state=TEST_FAILURE_STATE;
					break;
				
					
				case TEST_END_STATE:
				//	printf("enter end\n");
					if ('\0' == endPtr[0]) {
						parse_state=TEST_SUCESS_STATE;
						
						}
					else
						parse_state=TEST_FAILURE_STATE;
					break;
					
				case TEST_FAILURE_STATE:
				//	printf("enter failure\n");
					return -1;
					
					break;
					
				case TEST_SUCESS_STATE:
				//	printf("enter sucess\n");
					
					return 0;
					break;
					
				default:
					break;
			}
	}
	
}

int _parse_port(char* ptr,/*int* slot,int* port*/SLOT_PORT_VLAN_ENABLE* sp){

	char* endPtr = NULL;
    short slot = 0,port = 0;
	PARSE_PORT_STATE parse_state=TEST_SLOT_STATE;
	
    endPtr = ptr;

	//use a state machine to parse the port.  six states.
	while(1){
			switch(parse_state)
			{
				case TEST_SLOT_STATE:
					
					slot = strtoul(endPtr,&endPtr,10);
				//	printf("enter slot %d\n",slot);
					if(slot>0&&slot<=4){
            			sp->slot = slot;
						parse_state=TEST_SPLIT_STATE;
					}
					else
						parse_state=TEST_FAILURE_STATE;
					break;
					
				case TEST_SPLIT_STATE:
				//	printf("enter split\n");
					if (SLOT_PORT_SPLIT_SLASH== endPtr[0]||SLOT_PORT_SPLIT_DASH == endPtr[0]) 
						parse_state=TEST_PORT_STATE;
					else
						parse_state=TEST_FAILURE_STATE;
					break;
					
				case TEST_PORT_STATE:
					
					port = strtoul((char *)&(endPtr[1]),&endPtr,10);
				//	printf("enter port %d\n",port);
					if(port>0&&port<=24){
               			sp->port = port;
						parse_state=TEST_END_STATE;
					}
           			else
						parse_state=TEST_FAILURE_STATE;
					break;
				
					
				case TEST_END_STATE:
				//	printf("enter end\n");
					if ('\0' == endPtr[0]) {
						parse_state=TEST_SUCESS_STATE;
						
						}
					else
						parse_state=TEST_FAILURE_STATE;
					break;
					
				case TEST_FAILURE_STATE:
				//	printf("enter failure\n");
					return -1;
					
					break;
					
				case TEST_SUCESS_STATE:
				//	printf("enter sucess\n");
					
					return 0;
					break;
					
				default:
					break;
			}
	}
	
}


int _parse_port_list(char* ptr,int* count,SLOT_PORT_S spL[])
{

	char* endPtr = NULL;
    short tmpslot = 0,tmpport = 0;
    int   i=0;
	int   isSame=0;////
	PARSE_PORT_STATE parse_state=TEST_SLOT_STATE;

	
    endPtr = ptr;

	//use a state machine to parse the port list.  seven states here.
	//this parse function can delete the same items in the list.

	while(1){

		switch(parse_state)
			{
				case TEST_SLOT_STATE:
					
					tmpslot = strtoul(endPtr,&endPtr,10);
					if(SLOT_LLEGAL(tmpslot)){
            			spL[i].slot = tmpslot;
						parse_state=TEST_SPLIT_STATE;
					}
					else
						parse_state=TEST_FAILURE_STATE;
					break;
					
				case TEST_SPLIT_STATE:
					if (SLOT_PORT_SPLIT_SLASH== endPtr[0]||SLOT_PORT_SPLIT_DASH == endPtr[0]) 
						parse_state=TEST_PORT_STATE;
					else
						parse_state=TEST_FAILURE_STATE;
					break;
					
				case TEST_PORT_STATE:
					tmpport = strtoul((char *)&(endPtr[1]),&endPtr,10);
					if(PORT_LLEGAL(tmpport)){
               			spL[i].port = tmpport;
						{
							int ii=0;
							for(ii=0;ii<i;ii++)
							if(spL[i].slot==spL[ii].slot&&spL[i].port==spL[ii].port)
								isSame=1;
						}
						parse_state=TEST_COMMA_STATE;
					}
           			else
						parse_state=TEST_FAILURE_STATE;
					break;
					
				case TEST_COMMA_STATE:
					if (SLOT_PORT_SPLIT_COMMA== endPtr[0]){
            				endPtr = (char*)endPtr + 1;
							if(isSame==1)
            					{
									isSame=0;
								}
							else
								i++;
								
							parse_state=TEST_SLOT_STATE;
						}
					else
						parse_state=TEST_END_STATE;
  					break;
					
				case TEST_END_STATE:
					if ('\0' == endPtr[0]) {
							parse_state=TEST_SUCESS_STATE;
							if(isSame==1)
								isSame=0;
							else
								i++;
						}
					else
						parse_state=TEST_FAILURE_STATE;
					break;
					
				case TEST_FAILURE_STATE:
					return -1;
					
					break;
					
				case TEST_SUCESS_STATE:
					*count = i;
					return 0;
					break;
					
				default:
					break;
			}
	}
	
}



void CheckSecurityType(char *type, unsigned int SecurityType){

	switch(SecurityType){

		case OPEN :
			strcpy(type, "open");
			break;
			
		case SHARED :
			strcpy(type, "shared");
			break;
		
		case IEEE8021X :
			strcpy(type, "802.1x");
			break;

		case WPA_P :
			strcpy(type, "wpa_p");
			break;
			
		case WPA2_P :
			strcpy(type, "wpa2_p");
			break;

		case WPA2_E :
			strcpy(type, "wpa2_e");
			break;
			
		case WPA_E :
			strcpy(type, "wpa_e");
			break;
		
		case MD5:
			strcpy(type, "md5");
			break;

		case WAPI_PSK:
			strcpy(type, "wapi_psk");
			break;

		case WAPI_AUTH:
			strcpy(type, "wapi_auth");
			break;
		
	}

}

void CheckEncryptionType(char *type, unsigned int EncryptionType){

	switch(EncryptionType){

		case NONE :
			strcpy(type, "none");
			break;
			
		case WEP :
			strcpy(type, "wep");
			break;
		
		case AES :
			strcpy(type, "aes");
			break;

		case TKIP :
			strcpy(type, "tkip");
			break;
		
		case SMS4 :
			strcpy(type, "sms4");
			break;
		
	}

}

int Check_IP_Format(char* str){
	char *endptr = NULL;
	int endptr1 = 0;
	char c;
	int IP,i;
	c = str[0];
	if (c>='0'&&c<='9'){
		IP= strtoul(str,&endptr,10);
		if(IP < 0||IP > 255)
			return ASD_UNKNOWN_ID;
		else if(((IP < 10)&&((endptr - str) > 1))||((IP < 100)&&((endptr - str) > 2))||((IP < 256)&&((endptr - str) > 3))){
			return ASD_UNKNOWN_ID;
		}
		for(i = 0; i < 3; i++){
			if(endptr[0] == '\0'||endptr[0] != '.')
				return ASD_UNKNOWN_ID;
			else{
				endptr1 = &endptr[1];
				IP= strtoul(&endptr[1],&endptr,10);				
				if(IP < 0||IP > 255)
					return ASD_UNKNOWN_ID;				
				else if(((IP < 10)&&((endptr - endptr1) > 1))||((IP < 100)&&((endptr - endptr1) > 2))||((IP < 256)&&((endptr - endptr1) > 3))){
					return ASD_UNKNOWN_ID;
				}
			}
		}
		if(endptr[0] == '\0' && IP > 0)
			return ASD_DBUS_SUCCESS;
		else
			return ASD_UNKNOWN_ID;
	}
	else
		return ASD_UNKNOWN_ID;

}


void CheckRekeyMethod(char *type, unsigned char SecurityType){
//	xm0701
	switch(SecurityType){

		case 0 :
			strcpy(type, "disable");
			break;
			
		case 1 :
			strcpy(type, "time_based");
			break;
		
		case 2 :
			strcpy(type, "packet_based");
			break;

		case 3 :
			strcpy(type, "both_based");
			break;
				
	}

}

void Free_security_head(struct dcli_security *sec)
{
	void (*dcli_init_free_func)(struct dcli_security *);
	if(NULL != ccgi_dl_handle)
	{
		dcli_init_free_func = dlsym(ccgi_dl_handle,"dcli_free_security_list");
		if(NULL != dcli_init_free_func)
		{
			dcli_init_free_func(sec);
		}
	}
}

/*����1ʱ������Free_security_head()�ͷſռ�*/
int show_security_list(dbus_parameter parameter, DBusConnection *connection,struct dcli_security **sec,int *security_num)/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																														   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{
	if(NULL == connection)
		return 0;

	unsigned int 	ret = 0;
	unsigned char 	num = 0;
	int retu;


	void*(*dcli_init_func)(
					DBusConnection *,
					int , 
					unsigned char *, 
					int ,
					unsigned int *
					);

	
	*sec = NULL;
	if(NULL != ccgi_dl_handle)
	{
		dcli_init_func = dlsym(ccgi_dl_handle,"show_security_list");
		if(NULL != dcli_init_func)
		{
			*sec = (*dcli_init_func)
				(
					connection, 
					parameter.instance_id, 
					&num, 
					parameter.local_id,
					&ret
				);
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
	
	*security_num = num;

	if(((ret == 0)&&(*sec)) || (ret == ASD_SECURITY_NOT_EXIST))
	{
		retu = 1;
	}
	else if (ret == ASD_DBUS_ERROR)
	{
		return SNMPD_CONNECTION_ERROR;	
	}
	else
	{
		retu = -1;
	}

	return retu;	
}

void Free_security_one(struct dcli_security *sec)
{
	void (*dcli_init_free_func)(struct dcli_security *);
	if(NULL != ccgi_dl_handle)
	{
		dcli_init_free_func = dlsym(ccgi_dl_handle,"dcli_free_security");
		if(NULL != dcli_init_free_func)
		{
			dcli_init_free_func(sec);
		}
	}
}

/*����1ʱ������Free_security_one()�ͷſռ�*/
int show_security_one(dbus_parameter parameter, DBusConnection *connection,int id,struct dcli_security **sec)
																			/*����0��ʾʧ�ܣ�,����1��ʾ�ɹ�������-1��ʾsecurity id should be 1 to WLAN_NUM-1*/
                                     					                    /*����-2��ʾSecurity id is not exited������-3��ʾerror*/
																			/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{	
	if(NULL == connection)
		return 0;

	unsigned int 	ret = 0;
	unsigned char 	security_id = 0;
	int retu;

	security_id = id;
	if(security_id >= WLAN_NUM || security_id == 0){
		return -1;
	}

	
	void*(*dcli_init_func)(
					DBusConnection *,
					int , 
					unsigned char , 
					int ,
					unsigned int *
					);

	*sec = NULL;
	if(NULL != ccgi_dl_handle)
	{
		dcli_init_func = dlsym(ccgi_dl_handle,"show_security_id");
		if(NULL != dcli_init_func)
		{
			*sec = (*dcli_init_func)
				(
					connection, 
					parameter.instance_id, 
					security_id, 
					parameter.local_id, 
					&ret
				);
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
		
	if ((ret == 0) && (*sec != NULL)) 
		retu = 1;
	else if (ret == ASD_SECURITY_NOT_EXIST)
		retu = -2;
	else if (ret == ASD_DBUS_ERROR)		
		retu = SNMPD_CONNECTION_ERROR;
	else
		retu = -3;
		
	return retu;
}


int create_security(dbus_parameter parameter, DBusConnection *connection,char *id, char *sec_name) /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾsecurity id�Ƿ�������-2��ʾsecurity ID existed������-3��ʾerror*/
																									/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{
	if(NULL == connection)
		return 0;
	
	if((NULL == id)||(NULL == sec_name))
		return 0;
	
	int ret,retu;
	unsigned char isAdd = 1;	
	unsigned char security_id;
	char *name = NULL;
	DBusMessage *query, *reply;	
	DBusMessageIter	 iter;
	DBusError err;
	
	isAdd = 1;			
	
	ret = parse_security_char_ID((char*)id, &security_id);
	if(ret != WID_DBUS_SUCCESS){
		return -1;
	}
	if(security_id >= WLAN_NUM || security_id == 0){
		return -1;
	}
	
	name = (char*)malloc(strlen(sec_name)+1);
	if(NULL == name)
		return 0;
	memset(name, 0, strlen(sec_name)+1);
	memcpy(name, sec_name, strlen(sec_name));
	
	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SECURITY_METHOD_ADD_DEL_SECURITY);
	/*query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
						ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SECURITY_METHOD_ADD_DEL_SECURITY);*/
	dbus_error_init(&err);

	dbus_message_append_args(query,
						DBUS_TYPE_BYTE,&isAdd,								
						DBUS_TYPE_BYTE,&security_id,
						DBUS_TYPE_STRING,&name,
						DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);
	
	dbus_message_unref(query);
	
	if (NULL == reply) {
		if (dbus_error_is_set(&err)) {
			dbus_error_free(&err);
		}
		FREE_OBJECT(name);
		return SNMPD_CONNECTION_ERROR;
	}
	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == 0)
		retu=1;
	else if(ret == ASD_SECURITY_BE_USED)
		retu=-2;
	else
		retu=-3;
	
	dbus_message_unref(reply);
	FREE_OBJECT(name);
	return retu;	
}

/*sec_type�ķ�Χ��"open","shared","802.1x","WPA_P","WPA2_P","WPA_E","WPA2_E","MD5","WAPI_PSK"��"WAPI_AUTH"*/
int security_type(dbus_parameter parameter, DBusConnection *connection,int id,char *sec_type) 
														   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown security type������-2��ʾsecurity id not exist*/
			                                               /*����-3��ʾThis Security Profile be used by some Wlans,please disable these Wlans first������-4��ʾerror������-5��ʾSecurity ID�Ƿ�*/
														   /*����-6��ʾThe radius heart test is on,turn it off first!*/
														   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{
	if(NULL == connection)
		return 0;
	
	if(NULL == sec_type)
		return 0;
	
    int ret,retu;
	unsigned char security_id;
	unsigned int type;
	DBusMessage *query, *reply;	
	DBusMessageIter	 iter;
	DBusError err;
	//security_id = id;
	if (!strcmp(sec_type,"open"))
		type = OPEN;			
	else if (!strcmp(sec_type,"shared"))
		type = SHARED;	
	else if (!strcmp(sec_type,"802.1x"))
		type = IEEE8021X;
	else if (!strcmp(sec_type,"WPA_P"))
		type = WPA_P;
	else if (!strcmp(sec_type,"WPA2_P"))
		type = WPA2_P;
	else if (!strcmp(sec_type,"WPA_E"))
		type = WPA_E;
	else if (!strcmp(sec_type,"WPA2_E"))
		type = WPA2_E;		
	else if (!strcmp(sec_type,"MD5"))
		type = MD5;		
	else if (!strcmp(sec_type,"WAPI_PSK"))
		type = WAPI_PSK;	
	else if (!strcmp(sec_type,"WAPI_AUTH"))
		type = WAPI_AUTH;	
	else 
	{		
		return -1;
	}
	
	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	security_id = id;
	if(security_id >= WLAN_NUM || security_id == 0){
		syslog(LOG_DEBUG,"security id in security_type is %d\n",security_id);
		return -5;
	}
	
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SECURITY_METHOD_SECURITY_TYPE);
	
	/*query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
						ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SECURITY_METHOD_SECURITY_TYPE);*/
	dbus_error_init(&err);

	dbus_message_append_args(query,
							 DBUS_TYPE_BYTE,&security_id,
							 DBUS_TYPE_UINT32,&type,
							 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);
	
	dbus_message_unref(query);
	
	if (NULL == reply) {
		if (dbus_error_is_set(&err)) {
			dbus_error_free(&err);
		}
		return SNMPD_CONNECTION_ERROR;
	}
	
	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
		retu=1;		
	else if (ret==ASD_SECURITY_TYPE_HAS_CHANGED)
		retu=1;		
	else if(ret == ASD_SECURITY_NOT_EXIST)			
		retu=-2;		
	else if(ret == ASD_SECURITY_WLAN_SHOULD_BE_DISABLE)	
		retu=-3;
	else if(ret == ASD_SECURITY_HEART_TEST_ON)
		retu=-6;
	else
	    retu=-4;
	
	dbus_message_unref(reply);
	return retu;
}

/*enc_type�ķ�Χ��"none","WEP","AES","TKIP"��"SMS4"*/
int encryption_type(dbus_parameter parameter, DBusConnection *connection,int id,char *enc_type) 
															  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown encryption type*/
			                                                  /*����-2��ʾencryption type dosen't match with security type������-3��ʾsecurity id not exist*/
			                                                  /*����-4��ʾThis Security Profile be used by some Wlans,please disable these Wlans first*/
			                                                  /*����-5��ʾerror������-6��ʾSecurity ID�Ƿ�������-7��ʾThe radius heart test is on,turn it off first!*/
															  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{	
	if(NULL == connection)
		return 0;
	
	if(NULL == enc_type)
		return 0;
	
    int ret,retu;
	unsigned char security_id;
	unsigned int type;
	DBusMessage *query, *reply;	
	DBusMessageIter	 iter;
	DBusError err;
	//security_id = id;
	
	if (!strcmp(enc_type,"none"))
		type = NONE;			
	else if (!strcmp(enc_type,"WEP"))
		type = WEP;	
	else if (!strcmp(enc_type,"AES"))
		type = AES;
	else if (!strcmp(enc_type,"TKIP"))
		type = TKIP;	
	else if (!strcmp(enc_type,"SMS4"))
		type = SMS4;
	else 
	{		
		return -1;
	}
	

	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	security_id = id;
	if(security_id >= WLAN_NUM || security_id == 0){
		syslog(LOG_DEBUG,"security id in encryption_type is %d\n",security_id);
		return -6;
	}
	
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SECURITY_METHOD_ENCRYPTION_TYPE);

	/*query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
						ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SECURITY_METHOD_ENCRYPTION_TYPE);*/
	dbus_error_init(&err);

	dbus_message_append_args(query,
							 DBUS_TYPE_BYTE,&security_id,
							 DBUS_TYPE_UINT32,&type,
							 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);
	
	dbus_message_unref(query);
	
	if (NULL == reply) {
		if (dbus_error_is_set(&err)) {
			dbus_error_free(&err);
		}
		return SNMPD_CONNECTION_ERROR;
	}
	
	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
	  retu=1;
	else if(ret==ASD_SECURITY_TYPE_HAS_CHANGED)
		retu=1;
	else if (ret == ASD_SECURITY_TYPE_NOT_MATCH_ENCRYPTION_TYPE)
		retu=-2;
	else if (ret == ASD_SECURITY_NOT_EXIST)			
		retu=-3;		
	else if(ret == ASD_SECURITY_WLAN_SHOULD_BE_DISABLE)		
		retu=-4;
	else if(ret == ASD_SECURITY_HEART_TEST_ON)
		retu=-7;
	else
		retu=-5;
	
	dbus_message_unref(reply);
	return retu;
}


/*service_type==1��ʾenable��service_type==0��ʾdisable*/
int extensible_authentication(dbus_parameter parameter, DBusConnection *connection,int id,int service_type)   
																			/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
				                                                        	/*����-1��ʾencryption type dosen't match with security type������-2��ʾsecurity id not exist*/
				                                                        	/*����-3��ʾThis Security Profile be used by some Wlans,please disable these Wlans first*/
				                                                        	/*����-4��ʾerror������-5��ʾSecurity ID�Ƿ�*/
																			/*����-6��ʾextensible auth is supported open or shared*/
																			/*����-7��ʾThe radius heart test is on,turn it off first!*/
																			/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{
	if(NULL == connection)
		return 0;

    int ret = ASD_DBUS_SUCCESS,retu;
	unsigned char security_id;
	unsigned int type;
	DBusMessage *query, *reply;	
	DBusMessageIter	 iter;
	DBusError err;
	//security_id = id;
	type = service_type;	

	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	security_id = id;
	if(security_id >= WLAN_NUM || security_id == 0){
		syslog(LOG_DEBUG,"security id in extensible_authentication is %d\n",security_id);
		return -5;
	}
	
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SECURITY_METHOD_EXTENSIBLE_AUTH);
	
	/*query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
						ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SECURITY_METHOD_EXTENSIBLE_AUTH);*/
	dbus_error_init(&err);

	dbus_message_append_args(query,
							 DBUS_TYPE_BYTE,&security_id,
							 DBUS_TYPE_UINT32,&type,
							 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);
	
	dbus_message_unref(query);
	
	if (NULL == reply) {
		if (dbus_error_is_set(&err)) {
			dbus_error_free(&err);
		}
		return SNMPD_CONNECTION_ERROR;
	}
	
	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
		retu = 1;
	else if (ret == ASD_SECURITY_TYPE_NOT_MATCH_ENCRYPTION_TYPE)
		retu = -1;
	else if (ret == ASD_EXTENSIBLE_AUTH_NOT_SUPPORT)
		retu = -6;
	else if (ret == ASD_SECURITY_NOT_EXIST)	
		retu = -2;
	else if(ret == ASD_SECURITY_WLAN_SHOULD_BE_DISABLE)	
		retu = -3;
	else if(ret == ASD_SECURITY_HEART_TEST_ON)
		retu = -7;
	else
		retu = -4;
	
	dbus_message_unref(reply);
	return retu;
}

/*service_type==1��ʾwired��service_type==0��ʾwireless*/
int radius_server(dbus_parameter parameter, DBusConnection *connection,int id,int service_type)
															/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
				                                            /*����-1��ʾencryption type dosen't match with security type������-2��ʾsecurity id not exist*/
				                                            /*����-3��ʾThis Security Profile be used by some Wlans,please disable these Wlans first*/
				                                            /*����-4��ʾerror������-5��ʾSecurity ID�Ƿ�*/
															/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{	
	if(NULL == connection)
		return 0;

    int ret = ASD_DBUS_SUCCESS,retu;
	unsigned char security_id;
	unsigned int type;
	DBusMessage *query, *reply;	
	DBusMessageIter	 iter;
	DBusError err;
	//security_id = id;
	type = service_type;
	
	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	security_id = id;
	if(security_id >= WLAN_NUM || security_id == 0){
		syslog(LOG_DEBUG,"security id in radius_server is %d\n",security_id);
		return -5;
	}
	
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SECURITY_METHOD_RADIUS_SERVER_SELECT);

	/*query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
						ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SECURITY_METHOD_RADIUS_SERVER_SELECT);*/
	dbus_error_init(&err);

	dbus_message_append_args(query,
							 DBUS_TYPE_BYTE,&security_id,
							 DBUS_TYPE_UINT32,&type,
							 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);
	
	dbus_message_unref(query);
	
	if (NULL == reply) {
		if (dbus_error_is_set(&err)) {
			dbus_error_free(&err);
		}
		return SNMPD_CONNECTION_ERROR;
	}
	
	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
		retu=1;
	else if (ret == ASD_SECURITY_TYPE_NOT_MATCH_ENCRYPTION_TYPE)
		retu=-1;
	else if (ret == ASD_SECURITY_NOT_EXIST)	
		retu=-2;
	else if(ret == ASD_SECURITY_WLAN_SHOULD_BE_DISABLE)	
		retu=-3;
	else
		retu=-4;
	
	dbus_message_unref(reply);
	return retu;
}


int	secondary_radius_acct(dbus_parameter parameter, DBusConnection *connection,int id,char *secuip,int secu_port,char *secret)
																							 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾsecurity type which you choose not supported 802.1X*/
																							 /*����-2��ʾChange radius info not permited������-3��ʾThis Security Profile be used by some Wlans,please disable these Wlans first*/
																							 /*����-4��ʾPlease use radius acct ip port shared_secret first,command failed������-5��ʾerror������-6��ʾSecurity ID�Ƿ�*/
																							 /*����-7��ʾThe radius heart test is on,turn it off first!������-8��ʾunknown port id*/
																							 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{	
	if(NULL == connection)
        return 0;
	
	if((NULL == secuip)||(NULL == secret))
		return 0;
	
	int ret,retu;
	unsigned char security_id;
	unsigned int port;
	char *ip, *shared_secret;
	DBusMessage *query, *reply;	
	DBusMessageIter	 iter;
	DBusError err;	
	//security_id = id;

	ip = (char *)malloc(strlen(secuip)+1);     //don't forget free ip;
	if(NULL == ip)
		return 0;
	memset(ip, 0, strlen(secuip)+1);
	memcpy(ip, secuip, strlen(secuip));

	port = secu_port;
	if(port > 65535){
		FREE_OBJECT(ip);
		return -8;
	}	
	
	shared_secret = (char *)malloc(strlen(secret)+1);
	if(NULL == shared_secret)
		return 0;
	memset(shared_secret, 0, strlen(secret)+1);
	memcpy(shared_secret, secret, strlen(secret));	

	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	security_id = id;
	if(security_id >= WLAN_NUM || security_id == 0){
		syslog(LOG_DEBUG,"security id in secondary_radius_acct is %d\n",security_id);
		return -6;
	}
	
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SECURITY_METHOD_SECONDARY_SET_ACCT);

	/*send message to asd*/
	/*query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
						ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SECURITY_METHOD_SECONDARY_SET_ACCT);*/
	
	dbus_error_init(&err);

	dbus_message_append_args(query,
							 DBUS_TYPE_BYTE,&security_id,
							 DBUS_TYPE_UINT32,&port,
							 DBUS_TYPE_STRING,&ip,
							 DBUS_TYPE_STRING,&shared_secret,
							 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);

	//after send message ,free() some thing
	FREE_OBJECT(ip);	
	FREE_OBJECT(shared_secret);	
	
	dbus_message_unref(query);
	
	if (NULL == reply) {
		if (dbus_error_is_set(&err)) {
			dbus_error_free(&err);
		}
		return SNMPD_CONNECTION_ERROR;
	}

	/*get reply*/

	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
		retu = 1;
	else if(ret == ASD_SECURITY_TYPE_WITHOUT_8021X)			
		retu = -1;
	else if((ret == ASD_SECURITY_ACCT_BE_USED)||(ret == ASD_SECURITY_AUTH_BE_USED))
		retu = -2;
	else if(ret == ASD_SECURITY_WLAN_SHOULD_BE_DISABLE)			
		retu = -3;
	else if(ret == ASD_SECURITY_AUTH_NOT_EXIST)			
		retu = -4;
	else if(ret == ASD_SECURITY_HEART_TEST_ON)
		retu = -7;
	else
		retu = -5;

	dbus_message_unref(reply);
	
	return retu;

}


int security_host_ip(dbus_parameter parameter, DBusConnection *connection,int id,char * ip_addr)
															 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown ip format*/
					                                         /*����-2��ʾcheck_local_ip error������-3��ʾnot local ip������-4��ʾsecurity profile does not exist*/
				                                             /*����-5��ʾthis security profile is used by some wlans,please disable them first������-6��ʾerror������-7��ʾSecurity ID�Ƿ�*/
															 /*����-8��ʾThe radius heart test is on,turn it off first!*/
															 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{
	if(NULL == connection)
		return 0;
	
	if(NULL == ip_addr)
		return 0;
	
    int ret,ret1;
	int retu = 0;
	unsigned char security_id;
	char *ip;
	DBusMessage *query, *reply;	
	DBusMessageIter	 iter;
	DBusError err;
	//security_id = (int)id;

	ret1 = Check_IP_Format((char*)ip_addr);
	if(ret1 != ASD_DBUS_SUCCESS){
		return -1;
	}
	
	#if 0 
	ret2 = Check_Local_IP((char*)ip_addr);
	if (ret2 == -1){
		return -2;
	}else if(ret2 == 0){
		return -3;
	}
	#endif
	
	ip = (char *)malloc(strlen(ip_addr)+1);
	if(NULL == ip)
		return 0;
	memset(ip, 0, strlen(ip_addr)+1);
	memcpy(ip, ip_addr, strlen(ip_addr));
	
	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	security_id = (int)id;
	if(security_id >= WLAN_NUM || security_id == 0){
		syslog(LOG_DEBUG,"security id in security_host_ip is %d\n",security_id);
		return -7;
	}
	
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SECURITY_METHOD_SECURITY_HOST_IP);
	
	/*query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
						ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SECURITY_METHOD_SECURITY_HOST_IP);*/
	dbus_error_init(&err);

	dbus_message_append_args(query,
							 DBUS_TYPE_BYTE,&security_id,
							 DBUS_TYPE_STRING,&ip,
							 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);
	
	dbus_message_unref(query);
	
	FREE_OBJECT(ip);

	if (NULL == reply) {
		if (dbus_error_is_set(&err)) {
			dbus_error_free(&err);
		}
		return SNMPD_CONNECTION_ERROR;
	}
	
	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
		retu = 1;
	else if(ret == ASD_SECURITY_NOT_EXIST)	
		retu = -4;
	else if(ret == ASD_SECURITY_WLAN_SHOULD_BE_DISABLE)			
		retu = -5;
	else if(ret == ASD_SECURITY_HEART_TEST_ON)
		retu = -8;
	else
		retu = -6;
	
	dbus_message_unref(reply);
	return retu;
}


int secondary_radius_auth(dbus_parameter parameter ,DBusConnection *connection,int id,char * secuip,int secport,char *secr)
																						   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾsecurity type which you choose not supported 802.1X*/
																						   /*����-2��ʾChange radius info not permited������-3��ʾThis Security Profile be used by some Wlans,please disable these Wlans first*/
																						   /*����-4��ʾPlease use radius auth ip port shared_secret first,command failed������-5��ʾerror������-6��ʾSecurity ID�Ƿ�*/
																						   /*����-7��ʾThe radius heart test is on,turn it off first!������-8��ʾunknown port id*/
																						   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{	
	if(NULL == connection)
        return 0;
	
	if((NULL == secuip)||(NULL == secr))
		return 0;
	
	int ret,retu;
	unsigned char security_id;
	unsigned int port;
	char *ip, *shared_secret;
	DBusMessage *query, *reply;	
	DBusMessageIter	 iter;
	DBusError err;
	
	//security_id = id;

	/*get parameter for argv[ ]*/

	ip = (char *)malloc(strlen(secuip)+1);     //don't forget free ip;
	if(NULL == ip)
		return 0;
	memset(ip, 0, strlen(secuip)+1);
	memcpy(ip, secuip, strlen(secuip));

	port = secport;
	if(port > 65535){
		FREE_OBJECT(ip);
		return -8;
	}

	shared_secret = (char *)malloc(strlen(secr)+1);
	if(NULL == shared_secret)
		return 0;
	memset(shared_secret, 0, strlen(secr)+1);
	memcpy(shared_secret, secr, strlen(secr));

	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	security_id = id;
	if(security_id >= WLAN_NUM || security_id == 0){
		syslog(LOG_DEBUG,"security id in secondary_radius_auth is %d\n",security_id);
		return -6;
	}
	
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SECURITY_METHOD_SECONDARY_SET_AUTH);

	/*send message to asd*/
	/*query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
						ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SECURITY_METHOD_SECONDARY_SET_AUTH);*/
	
	dbus_error_init(&err);

	dbus_message_append_args(query,
							 DBUS_TYPE_BYTE,&security_id,
							 DBUS_TYPE_UINT32,&port,
							 DBUS_TYPE_STRING,&ip,
							 DBUS_TYPE_STRING,&shared_secret,
							 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);

	//after send message ,free() some thing
	FREE_OBJECT(ip);
	FREE_OBJECT(shared_secret);
	
	dbus_message_unref(query);
	
	
	if (NULL == reply) {
			if (dbus_error_is_set(&err)) {
				dbus_error_free(&err);
			}
			return SNMPD_CONNECTION_ERROR;
		}
	

	/*get reply*/

	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
		retu =1;
	else if(ret == ASD_SECURITY_TYPE_WITHOUT_8021X)			
		retu = -1;
	else if((ret == ASD_SECURITY_ACCT_BE_USED)||(ret == ASD_SECURITY_AUTH_BE_USED))
		retu = -2;
	else if(ret == ASD_SECURITY_WLAN_SHOULD_BE_DISABLE)			
		retu = -3;
	else if(ret == ASD_SECURITY_AUTH_NOT_EXIST)			
		retu = -4;
	else if(ret == ASD_SECURITY_HEART_TEST_ON)
		retu = -7;
	else
		retu = -5;

	dbus_message_unref(reply);
	
	return retu;

}


/*stateΪenable��disable*/
int config_vlan_list_enable_cmd_func(char *vlanlist,char *state)/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter is illegal������-2��ʾerror*/
{	
	if((NULL == vlanlist)||(NULL == state))
		return 0;
	
	int ret,retu;
	DBusMessage *query, *reply;	
	DBusMessageIter	 iter;
	DBusMessageIter  iter_array;
	DBusError err;
	int i=0;
	int list[4095];
	int n,num,stat=0;
	int num_from_npd=0;
	
	VLAN_ENABLE vlano[MAX_VLAN_NUM];
	memset(vlano,0,sizeof(VLAN_ENABLE)*MAX_VLAN_NUM);
	
	VLAN_PORT_ENABLE rcv_npd[MAX_VLAN_NUM];
	memset(rcv_npd,0,sizeof(VLAN_PORT_ENABLE)*MAX_VLAN_NUM);

	dbus_error_init(&err);
	ret = parse_vlan_list((char*)vlanlist,&n,list);
	num = RemoveListRepId(list,n);
	for(i=0;i<num;i++){
		vlano[i].vlanid=list[i];
	}
	if (-1 == ret) {
		return -1;
	}
	str2lower(&state);
	if (!strcmp(state,"enable")||(tolower(state[0]) == 'e'))
		stat=1;
	else
		stat=0;
	
	query = dbus_message_new_method_call(WID_DBUS_BUSNAME,WID_DBUS_WTP_OBJPATH,\
									WID_DBUS_WTP_INTERFACE,WID_DBUS_WTP_METHOD_SET_VLAN_LIST_ENABLE);					
	
	dbus_message_iter_init_append (query, &iter);

		
	dbus_message_iter_append_basic (	&iter,
										DBUS_TYPE_UINT32,
										& num);
		
	dbus_message_iter_open_container (&iter,
									DBUS_TYPE_ARRAY,
									DBUS_STRUCT_BEGIN_CHAR_AS_STRING
									DBUS_TYPE_UINT32_AS_STRING
									DBUS_TYPE_UINT32_AS_STRING
									DBUS_STRUCT_END_CHAR_AS_STRING,
									&iter_array);

	for(i = 0; i < num; i++){			
		DBusMessageIter iter_struct;
			
		dbus_message_iter_open_container (&iter_array,
										DBUS_TYPE_STRUCT,
										NULL,
										&iter_struct);

			
		dbus_message_iter_append_basic
					(&iter_struct,
					  DBUS_TYPE_UINT32,
					  &(vlano[i].vlanid));
		
		dbus_message_iter_append_basic
					(&iter_struct,
					  DBUS_TYPE_UINT32,
					  &stat);

		dbus_message_iter_close_container (&iter_array, &iter_struct);

	}
				
	dbus_message_iter_close_container (&iter, &iter_array);

	reply = dbus_connection_send_with_reply_and_block (ccgi_dbus_connection,query,-1, &err);

	
	dbus_message_unref(query);

	if (NULL == reply)
	{
		if (dbus_error_is_set(&err))
		{
			dbus_error_free(&err);
		}
		return 0;
	}

	/*receive message from npd*/
	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&num_from_npd);
	
	if(num_from_npd> 0 ){
			
		dbus_message_iter_next(&iter);	
		dbus_message_iter_recurse(&iter,&iter_array);
		

		for (i = 0; i < num_from_npd; i++) {
			DBusMessageIter iter_struct;
					
			dbus_message_iter_recurse(&iter_array,&iter_struct);
				
			dbus_message_iter_get_basic(&iter_struct,&(rcv_npd[i].vlanid));
				
			dbus_message_iter_next(&iter_struct);
					
			dbus_message_iter_get_basic(&iter_struct,&(rcv_npd[i].port));
	
			dbus_message_iter_next(&iter_array);

			rcv_npd[i].stat=stat;
			
		}
	
	}
		


	/*send this to the asd(take wid as example first)*/

	query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
			ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SECURITY_METHOD_SET_VLAN_LIST_APPEND_ENABLE);
														
				
	dbus_message_iter_init_append (query, &iter);
		
				
	dbus_message_iter_append_basic (	&iter,
										DBUS_TYPE_UINT32,
										&num_from_npd);

	dbus_message_iter_open_container (&iter,
									DBUS_TYPE_ARRAY,
									DBUS_STRUCT_BEGIN_CHAR_AS_STRING
									DBUS_TYPE_UINT32_AS_STRING
									DBUS_TYPE_UINT32_AS_STRING
									DBUS_TYPE_UINT32_AS_STRING
									DBUS_STRUCT_END_CHAR_AS_STRING,
									&iter_array);


	for(i = 0; i <num_from_npd; i++){			
		DBusMessageIter iter_struct;
			
		dbus_message_iter_open_container (&iter_array,
										DBUS_TYPE_STRUCT,
										NULL,
										&iter_struct);

			
		dbus_message_iter_append_basic
					(&iter_struct,
					  DBUS_TYPE_UINT32,
					  &(rcv_npd[i].vlanid));
		
		dbus_message_iter_append_basic
					(&iter_struct,
					  DBUS_TYPE_UINT32,
					  &rcv_npd[i].port);
		
		dbus_message_iter_append_basic
					(&iter_struct,
					  DBUS_TYPE_UINT32,
					  &rcv_npd[i].stat);

		dbus_message_iter_close_container (&iter_array, &iter_struct);

	}
				
	dbus_message_iter_close_container (&iter, &iter_array);

	reply = dbus_connection_send_with_reply_and_block (ccgi_dbus_connection,query,-1, &err);
			
			

		
	dbus_message_unref(query);
			
	if (NULL == reply)
	{
		if (dbus_error_is_set(&err))
		{
			dbus_error_free(&err);
		}
		dbus_message_unref(reply);
		return 0;
	}
			
	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);
	
	dbus_message_unref(reply);
	
	if(ret == 0)
		retu=1;		
	else
		retu=-2;

	return retu;

}

/*δʹ��*/
int config_vlan_list_security_cmd_func(char *vlanlist,char *secu_id)   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter illegal������-2��ʾunknown id format*/
																			/*����-3��ʾsecurity id should be 1 to WLAN_NUM-1������-4��ʾerror*/
																			/*����-5��ʾillegal input:Input exceeds the maximum value of the parameter type*/
{	
	if((NULL == vlanlist)||(NULL == secu_id))
		return 0;
	
	int ret,ret1,ret2,i,retu;
	int n,num,num1,sendnum;
	int list[4095];
	 
	VLAN_PORT_SECURITY vlani[MAX_VLAN_NUM],vlano[MAX_VLAN_NUM];
	memset(vlani,0,sizeof(VLAN_PORT_SECURITY)*MAX_VLAN_NUM);
	memset(vlano,0,sizeof(VLAN_PORT_SECURITY)*MAX_VLAN_NUM);	

	unsigned char security_id;
	DBusMessage *query, *reply;
	DBusError err;
	DBusMessageIter  iter,iiter,iiiter;
	DBusMessageIter	 iter_array,iiter_array,iiiter_array;
	
	/*get vlan ID*/
	ret2 = parse_vlan_list((char*)vlanlist,&n,list);

	//parse if there is repeat vlanid
	num = RemoveListRepId(list,n);

	for(i=0;i<num;i++){
		vlano[i].vlanid=list[i];
		}
	if (-1 == ret2) {
		return -1;
	}


	/*parse security id*/
	ret1 = parse_char_ID((char*)secu_id, &security_id);	
	if(ret1 != WID_DBUS_SUCCESS){
        if(ret1== WID_ILLEGAL_INPUT){
			retu = -5;
        }
		else{
			retu = -2;
		}
		return retu;
	}
	
	if(security_id >= WLAN_NUM || security_id == 0){
		return -3;
	}

	for(i=0;i<num;i++){
		vlano[i].securityid=security_id;
	}
	
	dbus_error_init(&err);

	query = dbus_message_new_method_call(WID_DBUS_BUSNAME,WID_DBUS_WTP_OBJPATH,\
								WID_DBUS_WTP_INTERFACE,WID_DBUS_WTP_METHOD_SET_SECURITY);
		
	dbus_message_iter_init_append (query, &iter);

		
	dbus_message_iter_append_basic (	&iter,
										DBUS_TYPE_UINT32,
										&num);
		
	dbus_message_iter_open_container (&iter,
									DBUS_TYPE_ARRAY,
									DBUS_STRUCT_BEGIN_CHAR_AS_STRING
									DBUS_TYPE_UINT32_AS_STRING					
									DBUS_STRUCT_END_CHAR_AS_STRING,
									&iter_array);

	for(i = 0; i < num; i++){			
		DBusMessageIter iter_struct;
			
		dbus_message_iter_open_container (&iter_array,
										DBUS_TYPE_STRUCT,
										NULL,
										&iter_struct);

			
		dbus_message_iter_append_basic
					(&iter_struct,
					  DBUS_TYPE_UINT32,
					  &(vlano[i].vlanid));
		

		dbus_message_iter_close_container (&iter_array, &iter_struct);
	}
					
	dbus_message_iter_close_container (&iter, &iter_array);
	

	reply = dbus_connection_send_with_reply_and_block (ccgi_dbus_connection,query,-1, &err);
		
	dbus_message_unref(query);
	if (NULL == reply) {
		if (dbus_error_is_set(&err)) {
			dbus_error_free(&err);
		}
		return 0;
	}
	dbus_message_iter_init(reply,&iiter);

	dbus_message_iter_get_basic(&iiter,&ret);
	
	if (ret==0){
		dbus_message_iter_next(&iiter);	
		dbus_message_iter_get_basic(&iiter,&num1);
		dbus_message_iter_next(&iiter);
		
		dbus_message_iter_recurse(&iiter,&iiter_array);
		
		for (i = 0; i <num1; i++) {
			
			DBusMessageIter iiter_struct;
			
		
			dbus_message_iter_recurse(&iiter_array,&iiter_struct);
	
			dbus_message_iter_get_basic(&iiter_struct,&(vlani[i].vlanid));
		
			dbus_message_iter_next(&iiter_struct);
		
			dbus_message_iter_get_basic(&iiter_struct,&(vlani[i].port));
				
			dbus_message_iter_next(&iiter_array);
		

			vlani[i].securityid = security_id;
			
		}
		sendnum = i;
	}
 	dbus_message_unref(reply);
	
	/*send this to the asd(take wid as example first)*/

	query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
								ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SECURITY_METHOD_SET_VLAN_APPEND_SECURITY);
															
		
	dbus_message_iter_init_append (query, &iiiter);

		
	dbus_message_iter_append_basic (	&iiiter,
										DBUS_TYPE_UINT32,
										& sendnum);

	
	
	
		
	dbus_message_iter_open_container (&iiiter,
									DBUS_TYPE_ARRAY,
									DBUS_STRUCT_BEGIN_CHAR_AS_STRING
									DBUS_TYPE_UINT32_AS_STRING
									DBUS_TYPE_UINT32_AS_STRING
									DBUS_TYPE_BYTE_AS_STRING
									DBUS_STRUCT_END_CHAR_AS_STRING,
									&iiiter_array);

	for(i = 0; i < sendnum; i++){			
		DBusMessageIter iiiter_struct;
			
		dbus_message_iter_open_container (&iiiter_array,
										DBUS_TYPE_STRUCT,
										NULL,
										&iiiter_struct);
		dbus_message_iter_append_basic
					(&iiiter_struct,
					   DBUS_TYPE_UINT32,
					  &(vlani[i].vlanid));
		dbus_message_iter_append_basic
					(&iiiter_struct,
					  DBUS_TYPE_UINT32,
					  &(vlani[i].port));
		dbus_message_iter_append_basic
					(&iiiter_struct,
					  DBUS_TYPE_BYTE,
					  &(vlani[i].securityid));

		dbus_message_iter_close_container (&iiiter_array, &iiiter_struct);

	}
						
	dbus_message_iter_close_container (&iiiter, &iiiter_array);

	reply = dbus_connection_send_with_reply_and_block (ccgi_dbus_connection,query,-1, &err);

	dbus_message_unref(query);
	
	if (NULL == reply)
	{
		if (dbus_error_is_set(&err))
		{
			dbus_error_free(&err);
		}
		return 0;
	}
	
	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);
	
	dbus_message_unref(reply);
	
	if(ret == 0)
		retu=1;
	else
		retu=-4;

	return retu;
}

/*δʹ��*/
int config_port_vlan_security_cmd_func(char *port_id,char *vlan_id,char *secu_id)/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter illegal*/
																						/*����-2��ʾunknown id format������-3��ʾvlan id should be 1 to 4094*/
																						/*����-4��ʾsecurity id should be 1 to WLAN_NUM-1������-5��ʾerror*/
																						/*����-6��ʾillegal input:Input exceeds the maximum value of the parameter type*/
{	
	if((NULL == port_id)||(NULL == vlan_id)||(NULL == secu_id))
		return 0;
	
	int vlanid;//,slot,port=0;
	unsigned char security_id;
	int ret,ret1,ret2,retu;	
	SLOT_PORT_VLAN_SECURITY vlan1,vlan2;		
	DBusMessage *query,*reply;
	DBusError err;
	DBusMessageIter  iter;
	DBusMessageIter	 iter_array;	
	
	ret = parse_port((char*)port_id,&vlan1);
	if (-1 == ret) {
		return -1;
	}
	
	ret1 = parse_int_ID((char*)vlan_id, &vlanid);
	
	vlan1.vlanid = vlanid;
	if(ret1 != WID_DBUS_SUCCESS){
		return -2;
	}
	if(vlanid<1||vlanid>4094){
		return -3;
	}

	//parse security id

	ret2 = parse_char_ID((char*)secu_id, &security_id);
	vlan1.securityid = security_id;
		
	if(ret2 != WID_DBUS_SUCCESS){
        if(ret2 == WID_ILLEGAL_INPUT){
			retu = -6;
        }
		else{
			retu = -2;
		}
		return retu;
	}
	if(security_id >= WLAN_NUM || security_id == 0){
		return -4;
	}
	
	dbus_error_init(&err);
	vlan2.slot = vlan1.slot;
	vlan2.port = vlan1.port;
	vlan2.vlanid = vlan1.vlanid;
	vlan2.portindex = vlan1.port;
	vlan2.securityid = security_id;
	
	query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
									ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SECURITY_METHOD_SET_PORT_VLAN_APPEND_SECURITY);

	dbus_message_iter_init_append (query, &iter);
				
	dbus_message_iter_open_container (&iter,
									DBUS_TYPE_ARRAY,
									DBUS_STRUCT_BEGIN_CHAR_AS_STRING
									DBUS_TYPE_UINT32_AS_STRING
									DBUS_TYPE_UINT32_AS_STRING
									DBUS_TYPE_UINT32_AS_STRING
									DBUS_TYPE_UINT32_AS_STRING
									DBUS_TYPE_BYTE_AS_STRING
									DBUS_STRUCT_END_CHAR_AS_STRING,
									&iter_array);
			
	DBusMessageIter iiter_struct;
		
	dbus_message_iter_open_container (&iter_array,
									DBUS_TYPE_STRUCT,
									NULL,
									&iiter_struct);
	dbus_message_iter_append_basic
				(&iiter_struct,
				   DBUS_TYPE_UINT32,
				  &(vlan2.slot));

	dbus_message_iter_append_basic
				(&iiter_struct,
				   DBUS_TYPE_UINT32,
				  &(vlan2.port));
	
	dbus_message_iter_append_basic
				(&iiter_struct,
				   DBUS_TYPE_UINT32,
				  &(vlan2.vlanid));
	
	dbus_message_iter_append_basic
				(&iiter_struct,
				   DBUS_TYPE_UINT32,
				  &(vlan2.portindex));

	dbus_message_iter_append_basic
				(&iiter_struct,
				   DBUS_TYPE_BYTE,
				  &(vlan2.securityid));

	dbus_message_iter_close_container (&iter_array, &iiter_struct);
			
	dbus_message_iter_close_container (&iter, &iter_array);

	reply = dbus_connection_send_with_reply_and_block (ccgi_dbus_connection,query,-1, &err);

	dbus_message_unref(query);

	if (NULL == reply)
	{
		if (dbus_error_is_set(&err))
		{
			dbus_error_free(&err);
		}
		return 0;
	}
	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);
	
	dbus_message_unref(reply);
			
	if(ret == 0)
		retu=1;		
	else
		retu=-5;

	return retu;
}


/*stateΪenable��disable*/
int config_port_vlan_enable_cmd_func(char *port_id,char *vlan_id,char *state)/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter illegal*/
																					/*����-2��ʾunknown id format������-3��ʾvlan id should be 1 to 4094*/
{	
	if((NULL == port_id)||(NULL == vlan_id)||(NULL == state))
		return 0;
	
	int vlanid;
	int stat;
	int ret,ret1,ret2;
	SLOT_PORT_VLAN_ENABLE vlan1,vlan2;		
	DBusMessage *query,*reply;
	DBusError err;
	DBusMessageIter  iter;
	DBusMessageIter	 iter_array;

	ret = _parse_port((char*)port_id,&vlan1);

	if (-1 == ret) {
		return -1;
	}

	ret1 = parse_int_ID((char*)vlan_id, &vlanid);
	vlan1.vlanid = vlanid;
	if(ret1 != WID_DBUS_SUCCESS){
		return -2;
	}
	if(vlanid<1||vlanid>4094){
		return -3;
	}

	str2lower(&state);
	if (!strcmp(state,"enable"))
		stat=1;
	else if (!strcmp(state,"disable"))
		stat=0;
	else	{
		return -2;
	}

	//send slot port vlanid to the acdbus
	dbus_error_init(&err);

	query = dbus_message_new_method_call(WID_DBUS_BUSNAME,WID_DBUS_WTP_OBJPATH,\
										WID_DBUS_WTP_INTERFACE,WID_DBUS_WTP_METHOD_SET_PORT_VLAN_ENABLE);
	



		
	dbus_message_iter_init_append (query, &iter);			
			
	dbus_message_iter_open_container (&iter,
									DBUS_TYPE_ARRAY,
									DBUS_STRUCT_BEGIN_CHAR_AS_STRING
									DBUS_TYPE_UINT32_AS_STRING
									DBUS_TYPE_UINT32_AS_STRING
									DBUS_TYPE_UINT32_AS_STRING
									DBUS_STRUCT_END_CHAR_AS_STRING,
									&iter_array);
			
	DBusMessageIter iter_struct;
		
	dbus_message_iter_open_container (&iter_array,
									DBUS_TYPE_STRUCT,
									NULL,
									&iter_struct);
		
	dbus_message_iter_append_basic(&iter_struct,
				  					DBUS_TYPE_UINT32,
				  					&(vlan1.slot));

	dbus_message_iter_append_basic(&iter_struct,
				  					DBUS_TYPE_UINT32,
				  					&(vlan1.port));

	dbus_message_iter_append_basic(&iter_struct,
				  					DBUS_TYPE_UINT32,
				  					&(vlan1.vlanid));
	

	dbus_message_iter_close_container (&iter_array, &iter_struct);

				
	dbus_message_iter_close_container (&iter, &iter_array);

	reply = dbus_connection_send_with_reply_and_block (ccgi_dbus_connection,query,-1, &err);
				
	dbus_message_unref(query);
	if (NULL == reply) {
		if (dbus_error_is_set(&err)) {
			dbus_error_free(&err);
		}
		return 0;
	}
	dbus_message_iter_init(reply,&iter);

	dbus_message_iter_get_basic(&iter,&ret2);
	
	if (ret2==0){
			
	dbus_message_iter_next(&iter);
	
	dbus_message_iter_recurse(&iter,&iter_array);
	
		DBusMessageIter iter_struct;
	
		dbus_message_iter_recurse(&iter_array,&iter_struct);

		dbus_message_iter_get_basic(&iter_struct,&(vlan2.vlanid));
	
		dbus_message_iter_next(&iter_struct);
	
		dbus_message_iter_get_basic(&iter_struct,&(vlan2.portindex));
			
		dbus_message_iter_next(&iter_array);

		vlan2.slot = vlan1.slot;
		vlan2.port = vlan1.port;;
		vlan2.stat = stat;
	}
 	dbus_message_unref(reply);
		
/*send this to the asd(take wid as example first)*/

	query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
								ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SECURITY_METHOD_SET_PORT_VLAN_APPEND_ENABLE);
		
	dbus_message_iter_init_append (query, &iter);
				
	dbus_message_iter_open_container (&iter,
									DBUS_TYPE_ARRAY,
									DBUS_STRUCT_BEGIN_CHAR_AS_STRING
									DBUS_TYPE_UINT32_AS_STRING
									DBUS_TYPE_UINT32_AS_STRING
									DBUS_TYPE_UINT32_AS_STRING
									DBUS_TYPE_UINT32_AS_STRING
									DBUS_TYPE_UINT32_AS_STRING
									DBUS_STRUCT_END_CHAR_AS_STRING,
									&iter_array);
			
	DBusMessageIter iiter_struct;
		
	dbus_message_iter_open_container (&iter_array,
									DBUS_TYPE_STRUCT,
									NULL,
									&iiter_struct);
	dbus_message_iter_append_basic
				(&iiter_struct,
				   DBUS_TYPE_UINT32,
				  &(vlan2.slot));

	dbus_message_iter_append_basic
				(&iiter_struct,
				   DBUS_TYPE_UINT32,
				  &(vlan2.port));
	
	dbus_message_iter_append_basic
				(&iiter_struct,
				   DBUS_TYPE_UINT32,
				  &(vlan2.vlanid));
	
	dbus_message_iter_append_basic
				(&iiter_struct,
				   DBUS_TYPE_UINT32,
				  &(vlan2.portindex));

	dbus_message_iter_append_basic
				(&iiter_struct,
				   DBUS_TYPE_UINT32,
				  &(vlan2.stat));

	dbus_message_iter_close_container (&iter_array, &iiter_struct);
			
	dbus_message_iter_close_container (&iter, &iter_array);

	reply = dbus_connection_send_with_reply_and_block (ccgi_dbus_connection,query,-1, &err);

	dbus_message_unref(query);
	
	if (NULL == reply)
	{
		if (dbus_error_is_set(&err))
		{
			dbus_error_free(&err);
		}
		return 0;
	}

		
 	dbus_message_unref(reply);

	return 1;
}

/*time�ķ�Χ��0-32767*/
int set_eap_reauth_period_cmd(dbus_parameter parameter, DBusConnection *connection,int id,int time)
																	/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																	/*����-1��ʾinput period value should be 0 to 32767*/
																	/*����-2��ʾsecurity profile does not exist*/
																	/*����-3��ʾThis Security Profile be used by some Wlans,please disable them first*/
																	/*����-4��ʾCan't set eap reauth period under current security type*/
																	/*����-5��ʾerror������-6��ʾSecurity ID�Ƿ�*/
																	/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{
	if(NULL == connection)
		return 0;

	int ret,retu;
	unsigned char security_id;
	DBusMessage *query, *reply; 
	DBusMessageIter  iter;
	DBusError err;	
	int period=3600;

	security_id = id;
	period= time;

	if(period<0||period>32767)
	{
		return -1;
	}

	
	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	security_id = id;
	if(security_id >= WLAN_NUM || security_id == 0){
		syslog(LOG_DEBUG,"security id in set_eap_reauth_period_cmd is %d\n",security_id);
		return -6;
	}
	
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SECURITY_METHOD_EAP_REAUTH_PERIOD);

	/*query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
					ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SECURITY_METHOD_EAP_REAUTH_PERIOD);*/
	dbus_error_init(&err);

	dbus_message_append_args(query,
						 DBUS_TYPE_BYTE,&security_id,
						 DBUS_TYPE_UINT32,&period,
						 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);

	dbus_message_unref(query);

	
	if (NULL == reply) {
		if (dbus_error_is_set(&err)) {
			dbus_error_free(&err);
		}
		return SNMPD_CONNECTION_ERROR;
	}
		
	

	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
		retu = 1;
	else if(ret == ASD_SECURITY_NOT_EXIST)			
		retu = -2;
	else if(ret == ASD_SECURITY_WLAN_SHOULD_BE_DISABLE) 		
		retu = -3;
 	else if(ret == ASD_SECURITY_TYPE_NOT_MATCH_ENCRYPTION_TYPE) 		
		retu = -4;
	else
		retu = -5;
	
	dbus_message_unref(reply);
	
	return retu;
}

/*inputype�ķ�Χ��"ascii"��"hex"*/
int security_key(dbus_parameter parameter, DBusConnection *connection,int id,char *SecurityKey,char*inputype)
																		   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾsecurity not exist*/
				                                                		   /*����-2��ʾsecurity key not permit set������-3��ʾkey length error������-4��ʾKey has been set up*/
				                                               			   /*����-5��ʾThis Security Profile be used by some Wlans,please disable these Wlans first������-6��ʾerror*/
																		   /*����-7��ʾhex key length error������-8��ʾkey format is incorrect(key should be '0' to '9' or 'a' to 'f')*/
																		   /*����-9��ʾSecurity ID�Ƿ�*/
																		   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{
	if(NULL == connection)
		return 0;
	
	if((NULL == SecurityKey)||(NULL == inputype))
		return 0;
	
	int ret = ASD_DBUS_SUCCESS;
	int retu;
	unsigned char security_id;
	char *key;
	DBusMessage *query, *reply;	
	DBusMessageIter	 iter;
	DBusError err;
	unsigned char input_type_of_key;	//1--ASCII        2--Hex
	//security_id = id;

	if ((!strcmp(inputype,"ASCII"))||(!strcmp(inputype,"ascii")))
		input_type_of_key=1;			
	else if ((!strcmp(inputype,"HEX"))||(!strcmp(inputype,"hex")))
		input_type_of_key= 2;	
	else 
	{		
		return 0;
	}
	
	key = (char*)malloc(strlen(SecurityKey)+1);
	if(NULL == key)
		return 0;
	memset(key, 0, strlen(SecurityKey)+1);
	memcpy(key, SecurityKey, strlen(SecurityKey));	

	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	security_id = id;
	if(security_id >= WLAN_NUM || security_id == 0){
		syslog(LOG_DEBUG,"security id in security_key is %d\n",security_id);
		return -9;
	}
	
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SECURITY_METHOD_SECURITY_KEY);

	/*query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
						ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SECURITY_METHOD_SECURITY_KEY);*/
	
	dbus_error_init(&err);

	dbus_message_append_args(query,
							 DBUS_TYPE_BYTE,&security_id,
							 DBUS_TYPE_STRING,&key,
						     DBUS_TYPE_BYTE,&input_type_of_key,
							 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);
	
	dbus_message_unref(query);
	
	FREE_OBJECT(key);//don't forget it.

	if (NULL == reply) {
		if (dbus_error_is_set(&err)) {
			dbus_error_free(&err);
		}
		return SNMPD_CONNECTION_ERROR;
	}
	
	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
		retu=1;
	else if(ret == ASD_SECURITY_NOT_EXIST)
		retu=-1;
	else if(ret == ASD_SECURITY_KEY_NOT_PERMIT)
	    retu=-2;
	else if(ret == ASD_SECURITY_KEY_LEN_NOT_PERMIT)			
		retu=-3;
	else if(ret == ASD_SECURITY_KEY_LEN_NOT_PERMIT_HEX)
		retu = -7;
	else if(ret == ASD_SECURITY_KEY_HEX_FORMAT)
		retu = -8;
	else if(ret == ASD_SECURITY_KEY_HAS_BEEN_SET)
		retu=-4;		
	else if(ret == ASD_SECURITY_WLAN_SHOULD_BE_DISABLE)	
		retu=-5;
	else 
		retu=-6;
		
	dbus_message_unref(reply);
	return retu;
}

/*aflag==0��ʾ"acct"��aflag==1��ʾ"auth"*/
int security_auth_acct(dbus_parameter parameter, DBusConnection *connection,int aflag,int id,char *secu_ip,int secu_port,char *secret)
																									/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown port*/
			                                                                                        /*����-2��ʾsecurity type which you choose not supported 802.1X������-3��ʾChange radius info not permited*/
			                                                                                        /*����-4��ʾThis Security Profile be used by some Wlans,please disable these Wlans first������-5��ʾerror*/
			                                                                                        /*����-6��ʾSecurity ID�Ƿ�������-7��ʾThe radius heart test is on,turn it off first!*/
																									/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{
	if(NULL == connection)
		return 0;
	
	if((NULL == secu_ip)||(NULL == secret))
		return 0;
	
    int ret,retu;
	unsigned char security_id;
	unsigned int type, port;
	char *ip, *shared_secret;
	DBusMessage *query, *reply;	
	DBusMessageIter	 iter;
	DBusError err;
	//security_id = id;
	type=aflag;
	
	ip = (char *)malloc(strlen(secu_ip)+1);
	if(NULL == ip)
		return 0;
	memset(ip, 0, strlen(secu_ip)+1);
	memcpy(ip, secu_ip, strlen(secu_ip));
	
	port = secu_port;
	
	if(port > 65535){
		FREE_OBJECT(ip);
		return -1;
	}		
	
	shared_secret = (char *)malloc(strlen(secret)+1);
	if(NULL == shared_secret)
		return 0;
	memset(shared_secret, 0, strlen(secret)+1);
	memcpy(shared_secret, secret, strlen(secret));	

	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	security_id = id;
	if(security_id >= WLAN_NUM || security_id == 0){
		syslog(LOG_DEBUG,"security id in security_auth_acct is %d\n",security_id);
		return -6;
	}
	
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	
	if(type == 0){	
		query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SECURITY_METHOD_SET_ACCT);
		/*query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
						ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SECURITY_METHOD_SET_ACCT);*/
	}else if(type == 1){	
		query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SECURITY_METHOD_SET_AUTH);
		/*query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
						ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SECURITY_METHOD_SET_AUTH);*/
	}
	dbus_error_init(&err);

	dbus_message_append_args(query,
							 DBUS_TYPE_BYTE,&security_id,
							 DBUS_TYPE_UINT32,&port,
							 DBUS_TYPE_STRING,&ip,
							 DBUS_TYPE_STRING,&shared_secret,
							 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);
	
	dbus_message_unref(query);
	
	FREE_OBJECT(ip);
	FREE_OBJECT(shared_secret);
	
	if (NULL == reply) {
		if (dbus_error_is_set(&err)) {
			dbus_error_free(&err);
		}
		return SNMPD_CONNECTION_ERROR;
	}
	
	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
		retu=1;
	else if(ret == ASD_SECURITY_RADIUS_HEARTTEST_DEFAULT){
		retu=1;
		//vty_out(vty,"security %d successfully set %s server.\n",security_id,argv[0]);
		//vty_out(vty,"<Attention> The radius heart test type change into default (auth).\n");
	}
	else if(ret == ASD_SECURITY_TYPE_WITHOUT_8021X)			
		retu=-2;		
	else if((ret == ASD_SECURITY_ACCT_BE_USED)||(ret == ASD_SECURITY_AUTH_BE_USED))
		retu=-3;		
	else if(ret == ASD_SECURITY_WLAN_SHOULD_BE_DISABLE)	
		retu=-4;
	else if(ret == ASD_SECURITY_HEART_TEST_ON)
		retu=-7;
	else
		retu=-5;
	
	dbus_message_unref(reply);
	return retu;
}


int delete_security(dbus_parameter parameter, DBusConnection *connection,int id)   /*����0��ʾ ɾ��ʧ�ܣ�����1��ʾɾ���ɹ�*/
																					/*����-1��ʾsecurity ID�Ƿ�������-2��ʾsecurity ID not existed*/
																					/*����-3��ʾThis Security Profile be used by some Wlans,please disable these Wlans first*/
																					/*����-4��ʾ��������-5��ʾThe radius heart test is on,turn it off first!*/
																					/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{
	if(NULL == connection)
		return 0;

	int ret,retu;
	unsigned char isAdd = 1;	
	unsigned char security_id;
	char *name = NULL;
	char *security_ip = NULL;
	char *name_d = "0";
	char *security_ip_d = "0";
	DBusMessage *query, *reply;	
	DBusMessageIter	 iter;
	DBusError err;
	
	isAdd = 0;
	security_id = id;	
	if(security_id >= WLAN_NUM || security_id == 0){
		return -1;
	}
	name = name_d;
	security_ip = security_ip_d;

	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SECURITY_METHOD_ADD_DEL_SECURITY);
	/*query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
						ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SECURITY_METHOD_ADD_DEL_SECURITY);*/
	dbus_error_init(&err);

	dbus_message_append_args(query,
						DBUS_TYPE_BYTE,&isAdd,								
						DBUS_TYPE_BYTE,&security_id,
						DBUS_TYPE_STRING,&name,
						DBUS_TYPE_STRING,&security_ip,							 
						DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);
	
	dbus_message_unref(query);
	
	if (NULL == reply) {
		if (dbus_error_is_set(&err)) {
			dbus_error_free(&err);
		}
		return SNMPD_CONNECTION_ERROR;
	}
	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == 0)
		retu=1;
	else if(ret == ASD_SECURITY_NOT_EXIST)
		retu=-2;
	else if(ret == ASD_SECURITY_WLAN_SHOULD_BE_DISABLE)			
		retu=-3;
	else if(ret == ASD_SECURITY_HEART_TEST_ON)
		retu=-5;
	else
		retu=-4;
	
	dbus_message_unref(reply);
	return retu;	
}

/*δʹ��*/
int config_port_cmd_func(char *post_list,char *secu_id)/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
															/*����-2��ʾsecurity id should be 1 to WLAN_NUM-1������-3��ʾunknown port format*/
															/*����-4��ʾillegal input:Input exceeds the maximum value of the parameter type*/
{
	if((NULL == post_list)||(NULL == secu_id))
		return 0;
	
	DBusMessage* reply;	
	DBusMessageIter	 iter,iiter,iiiter;
	DBusMessageIter	 iter_array,iiter_array,iiiter_array;
	DBusError err;
	int ret,ret1 ;
    int i=0;
	DBusMessage *query; 
	unsigned char 		security=0;
	unsigned int 	    p_count = 0  ;  //number of list from _parse_port_list
	unsigned int 		rcv_num=0;     //number of slot-port-vlanid items,receive from wid
	int retu;

	SLOT_PORT_S slotport[MAX_NUM_OF_VLANID] ;
	memset(slotport,0,sizeof(SLOT_PORT_S)*MAX_NUM_OF_VLANID);

	PORTINDEX_VLANID_S pvs[MAX_NUM_OF_VLANID];
	memset(pvs,0,sizeof(PORTINDEX_VLANID_S)*MAX_NUM_OF_VLANID);

	ret1 = parse_char_ID((char*)secu_id, &security);
	if(ret1 != WID_DBUS_SUCCESS){
		if(ret1 == WID_ILLEGAL_INPUT){
			retu = -4;
		}
		else{
			retu = -1;
		}
		return retu;
	}
	if(security >= WLAN_NUM || security == 0){
		return -2;
	}


	if (-1 == _parse_port_list((char *)post_list,&p_count,slotport)) {
		return -3;
	}

	if(p_count<=0||p_count>24){

		return -3;
	}
	
	dbus_error_init(&err);

	query = dbus_message_new_method_call(WID_DBUS_BUSNAME,WID_DBUS_WTP_OBJPATH,\
							WID_DBUS_WTP_INTERFACE,WID_DBUS_WTP_METHOD_CONFIG_PORT );
		
	dbus_message_iter_init_append (query, &iter);

		
	dbus_message_iter_append_basic (	&iter,
										DBUS_TYPE_UINT32,
										& p_count);
		
	dbus_message_iter_open_container (&iter,
									DBUS_TYPE_ARRAY,
									DBUS_STRUCT_BEGIN_CHAR_AS_STRING
									DBUS_TYPE_BYTE_AS_STRING
									DBUS_TYPE_BYTE_AS_STRING							
									DBUS_STRUCT_END_CHAR_AS_STRING,
									&iter_array);

	for(i = 0; i < p_count; i++){			
		DBusMessageIter iter_struct;
			
		dbus_message_iter_open_container (&iter_array,
										DBUS_TYPE_STRUCT,
										NULL,
										&iter_struct);

			
		dbus_message_iter_append_basic
					(&iter_struct,
					  DBUS_TYPE_BYTE,
					  &(slotport[i].slot));
		dbus_message_iter_append_basic
					(&iter_struct,
					  DBUS_TYPE_BYTE,
					  &(slotport[i].port));

		dbus_message_iter_close_container (&iter_array, &iter_struct);

	}
				
	dbus_message_iter_close_container (&iter, &iter_array);

	reply = dbus_connection_send_with_reply_and_block (ccgi_dbus_connection,query,-1, &err);

	
	dbus_message_unref(query);
	
	if (NULL == reply)
	{
		if (dbus_error_is_set(&err))
		{
			dbus_error_free(&err);
		}
		return 0;
	}

	/*code receive module*/
	
	dbus_message_iter_init(reply,&iiter);
	dbus_message_iter_get_basic(&iiter,&ret);
	if(ret == 0 ){
		dbus_message_iter_next(&iiter);	
		dbus_message_iter_get_basic(&iiter,&rcv_num);
		
	
	
		dbus_message_iter_next(&iiter);	
		dbus_message_iter_recurse(&iiter,&iiter_array);
		
		for (i = 0; i < rcv_num; i++) {
			DBusMessageIter iiter_struct;
			

			dbus_message_iter_recurse(&iiter_array,&iiter_struct);
		
			dbus_message_iter_get_basic(&iiter_struct,&(pvs[i].port_index));
		
			dbus_message_iter_next(&iiter_struct);
			
			dbus_message_iter_get_basic(&iiter_struct,&(pvs[i].vlan_id));
					
			dbus_message_iter_next(&iiter_array);

			pvs[i].security=security;

			
		}
	}
	 
	dbus_message_unref(reply);
	

	/*now we  send port_index-vlan_id-security to asd .*/

	query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
						ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SECURITY_METHOD_CONFIGURE_PORT);/////////
		
	dbus_message_iter_init_append (query, &iiiter);

		
	dbus_message_iter_append_basic (	&iiiter,
										DBUS_TYPE_UINT32,
										& rcv_num);

			
	dbus_message_iter_open_container (&iiiter,
									DBUS_TYPE_ARRAY,
									DBUS_STRUCT_BEGIN_CHAR_AS_STRING
									DBUS_TYPE_UINT32_AS_STRING
									DBUS_TYPE_UINT32_AS_STRING
									DBUS_TYPE_BYTE_AS_STRING
									DBUS_STRUCT_END_CHAR_AS_STRING,
									&iiiter_array);

	for(i = 0; i < rcv_num; i++){			
		DBusMessageIter iiiter_struct;
			
		dbus_message_iter_open_container (&iiiter_array,
										DBUS_TYPE_STRUCT,
										NULL,
										&iiiter_struct);
		dbus_message_iter_append_basic
					(&iiiter_struct,
					   DBUS_TYPE_UINT32,
					  &(pvs[i].port_index));
		dbus_message_iter_append_basic
					(&iiiter_struct,
					  DBUS_TYPE_UINT32,
					  &(pvs[i].vlan_id));
		dbus_message_iter_append_basic
					(&iiiter_struct,
					  DBUS_TYPE_BYTE,
					  &(pvs[i].security));

		dbus_message_iter_close_container (&iiiter_array, &iiiter_struct);

	}
				
	dbus_message_iter_close_container (&iiiter, &iiiter_array);

	reply = dbus_connection_send_with_reply_and_block (ccgi_dbus_connection,query,-1, &err);

	dbus_message_unref(query);
	
	if (NULL == reply)
	{
		if (dbus_error_is_set(&err))
		{
			dbus_error_free(&err);
		}
		return 0;
	}
		
	return 1; 	
}

/*stateΪenable��disable*/
int config_port_enable_cmd_func(char *post_list,char *state)/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknow port format������-2��ʾerror*/
{
	if((NULL == post_list)||(NULL == state))
		return 0;
	
	int ret,retu;
	DBusMessage *query, *reply;	
	DBusMessageIter	 iter;
	DBusMessageIter  iter_array;
	DBusError err;
	int i=0;
	unsigned int p_count=0;
	unsigned char stat=0;
	SLOT_PORT_S slotport[MAX_NUM_OF_VLANID] ;
	memset(slotport,0,sizeof(SLOT_PORT_S)*MAX_NUM_OF_VLANID);

	dbus_error_init(&err);
		
	str2lower(&state);
	if (!strcmp(state,"enable"))
		stat=1;
	else
		stat=0;

	/*receive port list in slotport array*/
	if (-1 == _parse_port_list((char *)post_list,&p_count,slotport)) {
			return -1;
		}

	/*now send  slot-port-stat to npd */
	query = dbus_message_new_method_call(WID_DBUS_BUSNAME,WID_DBUS_WTP_OBJPATH,\
							WID_DBUS_WTP_INTERFACE,WID_DBUS_WTP_METHOD_CONFIG_PORT_ENABLE );

	dbus_message_iter_init_append (query, &iter);

		
	dbus_message_iter_append_basic (	&iter,
										DBUS_TYPE_UINT32,
										& p_count);
		
	dbus_message_iter_open_container (&iter,
									DBUS_TYPE_ARRAY,
									DBUS_STRUCT_BEGIN_CHAR_AS_STRING
									DBUS_TYPE_BYTE_AS_STRING
									DBUS_TYPE_BYTE_AS_STRING
									DBUS_TYPE_BYTE_AS_STRING
									DBUS_STRUCT_END_CHAR_AS_STRING,
									&iter_array);

	for(i = 0; i < p_count; i++){			
		DBusMessageIter iter_struct;
			
		dbus_message_iter_open_container (&iter_array,
										DBUS_TYPE_STRUCT,
										NULL,
										&iter_struct);

			
		dbus_message_iter_append_basic
					(&iter_struct,
					  DBUS_TYPE_BYTE,
					  &(slotport[i].slot));
		dbus_message_iter_append_basic
					(&iter_struct,
					  DBUS_TYPE_BYTE,
					  &(slotport[i].port));
		dbus_message_iter_append_basic
					(&iter_struct,
					  DBUS_TYPE_BYTE,
					  &stat);

		dbus_message_iter_close_container (&iter_array, &iter_struct);

	}
				
	dbus_message_iter_close_container (&iter, &iter_array);

	reply = dbus_connection_send_with_reply_and_block (ccgi_dbus_connection,query,-1, &err);

	
	dbus_message_unref(query);
	
	if (NULL == reply)
	{
		if (dbus_error_is_set(&err))
		{
			dbus_error_free(&err);
		}
		return 0;
	}

	
	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	dbus_message_unref(reply);
	
	if(ret == 0)
		retu=1;
	else
		retu=-2;

	
		
	/*now send  slot-port-stat to asd */
	query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
						ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SECURITY_METHOD_CONFIGURE_PORT_ENABLE);
	
	dbus_message_iter_init_append (query, &iter);

		
	dbus_message_iter_append_basic (	&iter,
										DBUS_TYPE_UINT32,
										& p_count);
		
	dbus_message_iter_open_container (&iter,
									DBUS_TYPE_ARRAY,
									DBUS_STRUCT_BEGIN_CHAR_AS_STRING
									DBUS_TYPE_BYTE_AS_STRING
									DBUS_TYPE_BYTE_AS_STRING
									DBUS_TYPE_BYTE_AS_STRING
									DBUS_STRUCT_END_CHAR_AS_STRING,
									&iter_array);

	for(i = 0; i < p_count; i++){			
		DBusMessageIter iter_struct;
			
		dbus_message_iter_open_container (&iter_array,
										DBUS_TYPE_STRUCT,
										NULL,
										&iter_struct);

			
		dbus_message_iter_append_basic
					(&iter_struct,
					  DBUS_TYPE_BYTE,
					  &(slotport[i].slot));
		dbus_message_iter_append_basic
					(&iter_struct,
					  DBUS_TYPE_BYTE,
					  &(slotport[i].port));
		dbus_message_iter_append_basic
					(&iter_struct,
					  DBUS_TYPE_BYTE,
					  &stat);

		dbus_message_iter_close_container (&iter_array, &iter_struct);

	}
				
	dbus_message_iter_close_container (&iter, &iter_array);

	reply = dbus_connection_send_with_reply_and_block (ccgi_dbus_connection,query,-1, &err);

	
	dbus_message_unref(query);
	
	if (NULL == reply)
	{
		if (dbus_error_is_set(&err))
		{
			dbus_error_free(&err);
		}
		return 0;
	}

	
	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	dbus_message_unref(reply);
	
	if(ret == 0)
		retu=1;
	else
		retu=-2;

	return retu;

}


int set_acct_interim_interval(dbus_parameter parameter, DBusConnection *connection,int sid,int time)  
																   /*����0��ʾ ʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput time value should be 0 to 32767*/
																   /*����-2��ʾsecurity profile does not exist������-3��ʾThis Security Profile be used by some Wlans,please disable them first*/
																   /*����-4��ʾCan't set acct interim interval under current security type������-5��ʾSecurity ID�Ƿ�*/
																   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{
	if(NULL == connection)
		return 0;

	int ret,retu;
	unsigned char security_id;
	DBusMessage *query, *reply; 
	DBusMessageIter  iter;
	DBusError err;	
	int interval=0;

	//security_id = (int)sid;
	interval= time;

	if(interval<0||interval>32767)
	{
		return -1;
	}

	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	security_id = (int)sid;
	if(security_id >= WLAN_NUM || security_id == 0){
		syslog(LOG_DEBUG,"security id in set_acct_interim_interval is %d\n",security_id);
		return -5;
	}
	
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SECURITY_METHOD_ACCT_INTERIM_INTERVAL);

	/*query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
					ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SECURITY_METHOD_ACCT_INTERIM_INTERVAL);*/
	dbus_error_init(&err);

	dbus_message_append_args(query,
						 DBUS_TYPE_BYTE,&security_id,
						 DBUS_TYPE_UINT32,&interval,
						 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);

	dbus_message_unref(query);

	if (NULL == reply)
	{
		if (dbus_error_is_set(&err)) 
			{
				dbus_error_free(&err);
			}
		return SNMPD_CONNECTION_ERROR;
	}

	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
		retu=1;
	else if(ret == ASD_SECURITY_NOT_EXIST)
		retu=-2;
	else if(ret == ASD_SECURITY_WLAN_SHOULD_BE_DISABLE) 	
		retu=-3;
	else if(ret == ASD_SECURITY_TYPE_NOT_MATCH_ENCRYPTION_TYPE) 	
		retu=-4;
	else
		retu=0;
	
	dbus_message_unref(reply);
	
	return retu;
}

/*cer_type�ķ�Χ��"X.509"��"GBW"*/
int config_wapi_auth(dbus_parameter parameter, DBusConnection *connection,int sid,char *ip_addr,char *cer_type)   
																				/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown certification type*/
																				/*����-2��ʾunknown ip format*/
																				/*����-3��ʾsecurity type which you chose does not support wapi authentication*/
																				/*����-4��ʾthis security profile be used by some wlans,please disable them first*/
																				/*����-5��ʾerror������-6��ʾSecurity ID�Ƿ�*/
																				/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{	
	if(NULL == connection)
		return 0;
	
	if((NULL == ip_addr)||(NULL == cer_type))
		return 0;
	
	int ret,retu;
	unsigned char security_id;
	unsigned int type;
	char *ip;			
	DBusMessage *query, *reply; 
	DBusMessageIter  iter;
	DBusError err;
	
	//security_id = (int)sid;
	if (!strcmp(cer_type,"X.509"))
		type = 0;			
	else if (!strcmp(cer_type,"GBW"))
		type = 1;	
	else {
		return -1;
	}
	
	ret = Check_IP_Format((char*)ip_addr);
	if(ret != ASD_DBUS_SUCCESS){
		return -2;
	}
	ip = (char *)malloc(strlen(ip_addr)+1);
	if(NULL == ip)
		return 0;
	memset(ip, 0, strlen(ip_addr)+1);
	memcpy(ip, ip_addr, strlen(ip_addr));		
	
	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	security_id = (int)sid;
	if(security_id >= WLAN_NUM || security_id == 0){
		syslog(LOG_DEBUG,"security id in config_wapi_auth is %d\n",security_id);
		return -6;
	}
	
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SECURITY_METHOD_SET_WAPI_AUTH);
	/*query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
						ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SECURITY_METHOD_SET_WAPI_AUTH);*/

	dbus_error_init(&err);

	dbus_message_append_args(query,
							 DBUS_TYPE_BYTE,&security_id,
							 DBUS_TYPE_STRING,&ip,
							 DBUS_TYPE_UINT32,&type,
							 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);
	
	dbus_message_unref(query);

	FREE_OBJECT(ip);

	if (NULL == reply) {
		if (dbus_error_is_set(&err)) {
			dbus_error_free(&err);
		}
		return SNMPD_CONNECTION_ERROR;
	}
	
	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
		retu=1;
	else if(ret == ASD_SECURITY_TYPE_WITHOUT_WAPI_AUTH) 		
		retu=-3;
	else if(ret == ASD_SECURITY_WLAN_SHOULD_BE_DISABLE) 
		retu=-4;
	else
		retu=-5;

	dbus_message_unref(reply);
	return retu;
}

/*cer_type�ķ�Χ��"as","ae"��"ca"*/
int config_wapi_auth_path(dbus_parameter parameter, DBusConnection *connection,int sid,char *cer_type,char *path)   
																					/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																					/*����-1��ʾcertification isn't exit or can't be read*/
																					/*����-2��ʾsecurity type which you chose does not support wapi authentication*/
																					/*����-3��ʾthis security profile be used by some wlans,please disable them first*/
																					/*����-4��ʾthis security profile isn't integrity������-5��ʾerror������-6��ʾSecurity ID�Ƿ�*/
																					/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{	
	if(NULL == connection)
		return 0;
	
	if((NULL == cer_type)||(NULL == path))
		return  0;
	
	int ret,retu;
	unsigned char security_id;
	unsigned int type;
	char *cert_path;			
	DBusMessage *query, *reply;	
	DBusMessageIter	 iter;
	DBusError err;
	
	//security_id = (int)sid;
	if (!strcmp(cer_type,"as"))                 /*as--1 ; ae--2; ca--3*/
		type = 1;			
	else if (!strcmp(cer_type,"ae"))
		type = 2;
	else if (!strcmp(cer_type,"ca"))
		type = 3;

	if(access(path,R_OK) != 0){
		return -1;
	}
	
	cert_path = (char *)malloc(strlen(path)+1);
	if(NULL == cert_path)
		return 0;
	memset(cert_path, 0, strlen(path)+1);
	memcpy(cert_path, path, strlen(path));	

	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	security_id = sid;
	if(security_id >= WLAN_NUM || security_id == 0){
		syslog(LOG_DEBUG,"security id in config_wapi_auth_path is %d\n",security_id);
		return -6;
	}
	
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SECURITY_METHOD_SET_WAPI_PATH);

	/*query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
						ASD_DBUS_SECURITY_INTERFACE, ASD_DBUS_SECURITY_METHOD_SET_WAPI_PATH);*/

	dbus_error_init(&err);

	dbus_message_append_args(query,
							 DBUS_TYPE_BYTE,&security_id,
							 DBUS_TYPE_UINT32,&type,
							 DBUS_TYPE_STRING,&cert_path,
							 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);
	
	dbus_message_unref(query);


	FREE_OBJECT(cert_path);
	
	if (NULL == reply) {
		if (dbus_error_is_set(&err)) {
			dbus_error_free(&err);
		}
		return SNMPD_CONNECTION_ERROR;
	}
	
	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
		retu=1;
	else if(ret == ASD_SECURITY_TYPE_WITHOUT_WAPI_AUTH)			
		retu=-2;
	else if(ret == ASD_SECURITY_WLAN_SHOULD_BE_DISABLE)	
		retu=-3;		
	else if(ret == ASD_SECURITY_PROFILE_NOT_INTEGRITY)
		retu=-4;
	else
		retu=-5;

	dbus_message_unref(reply);
	return retu;
}

/*state�ķ�Χ��"enable"��"disable"*/
int config_pre_auth_cmd_func(dbus_parameter parameter, DBusConnection *connection,int sec_id,char *state)  
																			/*����0��ʾʧ�ܣ�����1��ʾ �ɹ�������-1��ʾunknown encryption type*/
																			/*����-2��ʾencryption type does not match security type������-3��ʾsecurity profile does not exist*/
																			/*����-4��ʾthis security profile is used by some wlans,please disable them first������-5��ʾerror*/
																			/*����-6��ʾSecurity ID�Ƿ�*/
																			/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{
	if(NULL == connection)
		return 0;
	
	if(NULL == state)
		return 0;
	
    int ret = ASD_DBUS_SUCCESS;
	int retu;
	unsigned char security_id;
	unsigned int type;
	DBusMessage *query, *reply;	
	DBusMessageIter	 iter;
	DBusError err;
	//security_id = sec_id;
	if ((!strcmp(state,"enable")))
		type = 1;			
	else if ((!strcmp(state,"disable")))
		type = 0;	
	else 
	{		
		return -1;
	}

	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	security_id = sec_id;
	if(security_id >= WLAN_NUM || security_id == 0){
		syslog(LOG_DEBUG,"security id in config_pre_auth_cmd_func is %d\n",security_id);
		return -6;
	}
	
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SECURITY_METHOD_PRE_AUTHENTICATION);
	
	/*query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
						ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SECURITY_METHOD_PRE_AUTHENTICATION);*/
	dbus_error_init(&err);

	dbus_message_append_args(query,
							 DBUS_TYPE_BYTE,&security_id,
							 DBUS_TYPE_UINT32,&type,
							 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);
	
	dbus_message_unref(query);
	
	if (NULL == reply) {
		if (dbus_error_is_set(&err)) {
			dbus_error_free(&err);
		}
		return SNMPD_CONNECTION_ERROR;
	}
	
	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
		retu=1;
	else if (ret == ASD_SECURITY_TYPE_NOT_MATCH_ENCRYPTION_TYPE)
		retu=-2;
	else if (ret == ASD_SECURITY_NOT_EXIST)	
		retu=-3;
	else if(ret == ASD_SECURITY_WLAN_SHOULD_BE_DISABLE)		
		retu=-4;
	else
		retu=-5;

	dbus_message_unref(reply);
	return retu;
}


/*UormΪ"unicast"��"multicast"��TorpΪ"time"��"packet"��paraΪ<0-400000000>*/
int set_wapi_rekey_para_cmd_func(dbus_parameter parameter, DBusConnection *connection,int id,char *Uorm,char *Torp,char *para)
																								/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown command format*/
																								/*����-2��ʾinput value should be 0 to 400000000������-3��ʾsecurity profile does not exist*/
																								/*����-4��ʾThis Security Profile be used by some Wlans,please disable them first*/
																								/*����-5��ʾCan't set wapi rekey parameter under current config������-6��ʾerror������-7��ʾSecurity ID�Ƿ�*/
																								/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{
	if(NULL == connection)
		return 0;
	
	if((NULL == Uorm)||(NULL == Torp)||(NULL == para))
		return 0;
	
	int ret,retu;
	unsigned char security_id;
	DBusMessage *query, *reply; 
	DBusMessageIter  iter;
	DBusError err;	
	unsigned int value=0;
	unsigned char uorm,torp;

	//security_id = id;

	str2lower(&Uorm);
	if (!strcmp(Uorm,"unicast"))
		uorm=0;
	else if (!strcmp(Uorm,"multicast"))
		uorm=1;
	else	{
		return -1;
	}

	str2lower(&Torp);
	if (!strcmp(Torp,"time"))
		torp=0;
	else if (!strcmp(Torp,"packet"))
		torp=1;
	else	{
		return -1;
	}
	
	value= atoi(para);

	if(value>400000000){
		return -2;
	}
	

	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	security_id = id;
	if(security_id >= WLAN_NUM || security_id == 0){
		syslog(LOG_DEBUG,"security id in set_wapi_rekey_para_cmd_func is %d\n",security_id);
		return -7;
	}
	
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SECURITY_METHOD_WAPI_REKEY_PARA);

	/*query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
					ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SECURITY_METHOD_WAPI_REKEY_PARA);////////////??????*/
	dbus_error_init(&err);

	dbus_message_append_args(query,
						 DBUS_TYPE_BYTE,&security_id,
						 DBUS_TYPE_BYTE,&uorm,
						 DBUS_TYPE_BYTE,&torp,
						 DBUS_TYPE_UINT32,&value,
						 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);

	dbus_message_unref(query);

	if (NULL == reply)
	{
		if (dbus_error_is_set(&err)) 
			{
				dbus_error_free(&err);
			}
		return SNMPD_CONNECTION_ERROR;
	}

	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
		retu=1;
	else if(ret == ASD_SECURITY_NOT_EXIST)			
		retu=-3;
	else if(ret == ASD_SECURITY_WLAN_SHOULD_BE_DISABLE) 		
		retu=-4;
	else if(ret == ASD_SECURITY_TYPE_NOT_MATCH_ENCRYPTION_TYPE) 	
		retu=-5;
	else
		retu=-6;
	
	dbus_message_unref(reply);	
	return retu;
}


/*UormΪ"unicast"��"multicast"��MethodΪ"disable"��"time_based"��"packet_based"��"both_based"*/
int set_wapi_ucast_rekey_method_cmd_func(dbus_parameter parameter, DBusConnection *connection,int id,char *Uorm,char *Method)
																								  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown command format*/
																								  /*����-2��ʾsecurity profile does not exist*/
																								  /*����-3��ʾThis Security Profile be used by some Wlans,please disable them first*/
																								  /*����-4��ʾCan't set wapi unicast rekey method under current security type*/
																								  /*����-5��ʾerror������-6��ʾSecurity ID�Ƿ�*/
																								  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{
	if(NULL == connection)
		return 0;
	
	if((NULL == Uorm)||(NULL == Method))
		return 0;
	
	int ret,retu;
	unsigned char security_id;
	DBusMessage *query, *reply; 
	DBusMessageIter  iter;
	DBusError err;	
	unsigned char method=1;	//	0--disable	1--time_based		2--packet_based	3--both_based
	unsigned char uorm=0;	//	0--unicast	1--multicast

	//security_id = id;

	str2lower(&Uorm);
	if (!strcmp(Uorm,"unicast"))
		uorm=0;
	else if (!strcmp(Uorm,"multicast"))
		uorm=1;
	else	{
		return -1;
	}
	
	str2lower(&Method);
	if (!strcmp(Method,"disable"))
		method=0;
	else if (!strcmp(Method,"time_based"))
		method=1;
	else if (!strcmp(Method,"packet_based"))
		method=2;
	else if (!strcmp(Method,"both_based"))
		method=3;
	else	{
		return -1;
	}
	

	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	security_id = id;
	if(security_id >= WLAN_NUM || security_id == 0){
		syslog(LOG_DEBUG,"security id in set_wapi_ucast_rekey_method_cmd_func is %d\n",security_id);
		return -6;
	}
	
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SECURITY_METHOD_WAPI_UCAST_REKEY_METHOD);
	
	/*query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
					ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SECURITY_METHOD_WAPI_UCAST_REKEY_METHOD);*/
	dbus_error_init(&err);

	dbus_message_append_args(query,
						 DBUS_TYPE_BYTE,&security_id,
						 DBUS_TYPE_BYTE,&uorm,
						 DBUS_TYPE_BYTE,&method,
						 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);

	dbus_message_unref(query);

	if (NULL == reply)
	{
		if (dbus_error_is_set(&err)) 
			{
				dbus_error_free(&err);
			}
		return SNMPD_CONNECTION_ERROR;
	}

	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
		retu=1;
	else if(ret == ASD_SECURITY_NOT_EXIST)			
		retu=-2;
	else if(ret == ASD_SECURITY_WLAN_SHOULD_BE_DISABLE) 
		retu=-3;
	else if(ret == ASD_SECURITY_TYPE_NOT_MATCH_ENCRYPTION_TYPE) 
		retu=-4;
	else
		retu=-5;
	
	dbus_message_unref(reply);	
	return retu;
}

/*periodΪ<0-65535>*/
int set_security_quiet_period_cmd_func(dbus_parameter parameter, DBusConnection *connection,int sec_id,char *period)
																					 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput time value should be 0 to 65535*/
																					 /*����-2��ʾsecurity profile does not exist������-3��ʾThis Security Profile be used by some Wlans,please disable them first*/
																					 /*����-4��ʾCan't set 1x quiet period under current security type������-5��ʾerror������-6��ʾSecurity ID�Ƿ�*/
																					 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{
	if(NULL == connection)
		return 0;
	
	if(NULL == period)
		return 0;
	
	int ret,retu;
	unsigned char security_id;
	DBusMessage *query, *reply; 
	DBusMessageIter  iter;
	DBusError err;
	unsigned int quietPeriod=0;

	//security_id = sec_id;
	quietPeriod= atoi(period);

	if(quietPeriod<0||quietPeriod>65535)
	{
		return -1;
	}

	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	security_id = sec_id;
	if(security_id >= WLAN_NUM || security_id == 0){
		syslog(LOG_DEBUG,"security id in set_security_quiet_period_cmd_func is %d\n",security_id);
		return -6;
	}
	
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SECURITY_METHOD_QUIET_PERIOD);

	/*query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
					ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SECURITY_METHOD_QUIET_PERIOD);*/
	dbus_error_init(&err);

	dbus_message_append_args(query,
						 DBUS_TYPE_BYTE,&security_id,
						 DBUS_TYPE_UINT32,&quietPeriod,
						 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);

	dbus_message_unref(query);

	if (NULL == reply)
	{
		if (dbus_error_is_set(&err)) 
			{
				dbus_error_free(&err);
			}
		return SNMPD_CONNECTION_ERROR;
	}

	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
		retu=1;
	else if(ret == ASD_SECURITY_NOT_EXIST)			
		retu=-2;
	else if(ret == ASD_SECURITY_WLAN_SHOULD_BE_DISABLE) 
		retu=-3;
	else if(ret == ASD_SECURITY_TYPE_NOT_MATCH_ENCRYPTION_TYPE) 
		retu=-4;
	else
		retu=-5;
	
	dbus_message_unref(reply);	
	return retu;
}

/*δʹ��*/
/*stateΪ"enable"��"disable"*/
int config_accounting_on_cmd(dbus_parameter parameter, DBusConnection *connection,int id,char *state)
																	  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown encryption type*/
																	  /*����-2��ʾsecurity type is needn't 802.1X������-3��ʾsecurity profile does not exist*/
																	  /*����-4��ʾthis security profile is used by some wlans,please disable them first*/
																	  /*����-5��ʾerrorthis security profile is used by some wlans,please disable them first*/
																	  /*����-6��ʾSecurity ID�Ƿ�*/
																	  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{	
	if(NULL == connection)
		return 0;
	
	if(NULL == state)
		return 0;
		
	int ret = ASD_DBUS_SUCCESS;
	unsigned char security_id;
	unsigned int type;
	DBusMessage *query, *reply; 
	DBusMessageIter  iter;
	DBusError err;
	int retu;
	//security_id = (int)vty->index;
	
	if ((!strcmp(state,"enable")))
		type = 0;			
	else if ((!strcmp(state,"disable")))
		type = 1;	
	else 
	{		
		//vty_out(vty,"<error> unknown encryption type.\n");
		return -1;
	}	

	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	security_id = id;
	if(security_id >= WLAN_NUM || security_id == 0){
		syslog(LOG_DEBUG,"security id in config_accounting_on_cmd is %d\n",security_id);
		return -6;
	}
	
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SECURITY_METHOD_ACCOUNTING_ON);
	/*query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
						ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SECURITY_METHOD_ACCOUNTING_ON);*/
	dbus_error_init(&err);

	dbus_message_append_args(query,
							 DBUS_TYPE_BYTE,&security_id,
							 DBUS_TYPE_UINT32,&type,
							 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);
	
	dbus_message_unref(query);
	
	if (NULL == reply) {
		if (dbus_error_is_set(&err)) {
			dbus_error_free(&err);
		}
		return SNMPD_CONNECTION_ERROR;
	}
	
	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
		retu = 1;
	else if (ret == ASD_SECURITY_TYPE_WITHOUT_8021X)
		retu = -2;
	else if (ret == ASD_SECURITY_NOT_EXIST) 	
		retu = -3;
	else if(ret == ASD_SECURITY_WLAN_SHOULD_BE_DISABLE) 	
		retu = -4;
	else
		retu = -5;

	dbus_message_unref(reply);
	return retu;
}

/*δʹ��*/
/*stateΪ"enable"��"disable"*/
int set_mobile_open_cmd(dbus_parameter parameter, DBusConnection *connection,char *state)/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput should be enable or disable������-2��ʾerror*/
																							   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{
	if(NULL == connection)
		return 0;
	
	if(NULL == state)
		return 0;
	
	int ret;
	DBusMessage *query, *reply; 
	DBusMessageIter  iter;
	DBusError err;
	int retu;
	unsigned char wtp_send_response_to_mobilephone=0;
	if(!strcmp(state,"enable")){
		wtp_send_response_to_mobilephone=1;
	}
	else if(!strcmp(state,"disable")){
		wtp_send_response_to_mobilephone=0;
	}
	else 
	{	//vty_out(vty,"<error> input should be enable or disable.\n");
		return -1;
	}	

	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_MOBILE_OPEN);

	/*query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
					ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_MOBILE_OPEN);*/
	dbus_error_init(&err);

	dbus_message_append_args(query,
						 DBUS_TYPE_BYTE,&wtp_send_response_to_mobilephone,
						 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);

	dbus_message_unref(query);

	if (NULL == reply)
	{
		if (dbus_error_is_set(&err)) 
		{
			dbus_error_free(&err);
		}
		return SNMPD_CONNECTION_ERROR;
	}

	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
		retu = 1;
	else
		retu = -2;
	
	dbus_message_unref(reply);
	
	return retu;
}

/*δʹ��*/
/*stateΪ"enable"��"disable"*/
int config_radius_extend_attr_cmd(dbus_parameter parameter, DBusConnection *connection,int id,char *state)
																		  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown encryption type*/
																		  /*����-2��ʾsecurity type is needn't 802.1X������-3��ʾsecurity profile does not exist*/
																		  /*����-4��ʾthis security profile is used by some wlans,please disable them first*/
																		  /*����-5��ʾerror������-6��ʾSecurity ID�Ƿ�*/
																		  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{	
	if(NULL == connection)
		return 0;
	
	if(NULL == state)
		return 0;
	
	int ret = ASD_DBUS_SUCCESS;
	unsigned char security_id;
	unsigned int type;
	DBusMessage *query, *reply; 
	DBusMessageIter  iter;
	DBusError err;
	int retu;
	//security_id = (int)vty->index;
	
	if ((!strcmp(state,"enable")))
		type = 1;			
	else if ((!strcmp(state,"disable")))
		type = 0;	
	else 
	{		
		//vty_out(vty,"<error> unknown encryption type.\n");
		return -1;
	}	

	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	security_id =  id;	
	if(security_id >= WLAN_NUM || security_id == 0){
		syslog(LOG_DEBUG,"security id in config_radius_extend_attr_cmd is %d\n",security_id);
		return -6;
	}
	
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SECURITY_METHOD_RADIUS_EXTEND_ATTR);
	/*query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
						ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SECURITY_METHOD_RADIUS_EXTEND_ATTR);*/
	dbus_error_init(&err);

	dbus_message_append_args(query,
							 DBUS_TYPE_BYTE,&security_id,
							 DBUS_TYPE_UINT32,&type,
							 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);
	
	dbus_message_unref(query);
	
	if (NULL == reply) {
		if (dbus_error_is_set(&err)) {
			dbus_error_free(&err);
		}
		return SNMPD_CONNECTION_ERROR;
	}
	
	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
		retu = 1;
	else if (ret == ASD_SECURITY_TYPE_WITHOUT_8021X)
		retu = -2;
	else if (ret == ASD_SECURITY_NOT_EXIST) 	
		retu = -3;
	else if(ret == ASD_SECURITY_WLAN_SHOULD_BE_DISABLE) 	
		retu = -4;
	else
		retu = -5;

	dbus_message_unref(reply);
	return retu;
}

/*stateΪ"enable"��"disable"*/
int set_wapi_sub_attr_wapipreauth_cmd(dbus_parameter parameter, DBusConnection *connection,int id,char *state)
																				 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾWapiPreauth should be enable or disable*/
																				 /*����-2��ʾsecurity profile does not exist������-3��ʾThis Security Profile be used by some Wlans,please disable them first*/
																				 /*����-4��ʾCan't set WapiPreauth under current security type������-5��ʾerror������-6��ʾSecurity ID�Ƿ�*/
																				 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{
	if(NULL == connection)
		return 0;
	
	if(NULL == state)
		return 0;
	
	int ret;
	unsigned char security_id;
	DBusMessage *query, *reply; 
	DBusMessageIter  iter;
	DBusError err;
	int retu;
	unsigned char WapiPreauth=0;

	//security_id = (int)vty->index;
	if ((!strcmp(state,"enable")))
		WapiPreauth = 1;			
	else if ((!strcmp(state,"disable")))
		WapiPreauth = 0;	
	else 
	{		
		return -1;
	}

	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	security_id =  id;	
	if(security_id >= WLAN_NUM || security_id == 0){
		syslog(LOG_DEBUG,"security id in set_wapi_sub_attr_wapipreauth_cmd is %d\n",security_id);
		return -6;
	}
	
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SECURITY_METHOD_WAPI_SUB_ATTR_WAPIPREAUTH_UPDATE);
	

	/*query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
					ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SECURITY_METHOD_WAPI_SUB_ATTR_WAPIPREAUTH_UPDATE);*/
	dbus_error_init(&err);

	dbus_message_append_args(query,
						 DBUS_TYPE_BYTE,&security_id,
						 DBUS_TYPE_BYTE,&WapiPreauth,
						 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);

	dbus_message_unref(query);

	if (NULL == reply)
	{
		if (dbus_error_is_set(&err)) 
		{
			dbus_error_free(&err);
		}
		return SNMPD_CONNECTION_ERROR;
	}

	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
		retu = 1;
	else if(ret == ASD_SECURITY_NOT_EXIST)		
		retu = -2;
	else if(ret == ASD_SECURITY_WLAN_SHOULD_BE_DISABLE) 	
		retu = -3;
	else if(ret == ASD_SECURITY_TYPE_NOT_MATCH_ENCRYPTION_TYPE) 	
		retu = -4;
	else
		retu = -5;
	
	dbus_message_unref(reply);	
	return retu;
}

/*stateΪ"enable"��"disable"*/
int set_wapi_sub_attr_multicaserekeystrict_cmd(dbus_parameter parameter, DBusConnection *connection,int id,char *state)
																						 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾMulticaseRekeyStrict should be enable or disable*/
																						 /*����-2��ʾsecurity profile does not exist������-3��ʾThis Security Profile be used by some Wlans,please disable them first*/
																						 /*����-4��ʾCan't set MulticaseRekeyStrict under current security type������-5��ʾerror������-6��ʾSecurity ID�Ƿ�*/
																						 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{
	if(NULL == connection)
		return 0;
	
	if(NULL == state)
		return 0;
	
	int ret;
	unsigned char security_id;
	DBusMessage *query, *reply; 
	DBusMessageIter  iter;
	DBusError err;
	int retu;	
	unsigned char MulticaseRekeyStrict=0;

	//security_id = (int)vty->index;
	if ((!strcmp(state,"enable")))
		MulticaseRekeyStrict = 1;			
	else if ((!strcmp(state,"disable")))
		MulticaseRekeyStrict = 0;	
	else 
	{		
		return -1;
	}
	
	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	security_id =  id;	
	if(security_id >= WLAN_NUM || security_id == 0){
		syslog(LOG_DEBUG,"security id in set_wapi_sub_attr_multicaserekeystrict_cmd is %d\n",security_id);
		return -6;
	}
	
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SECURITY_METHOD_WAPI_SUB_ATTR_MUTICASEREKEYSTRICT_UPDATE);

	/*query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
					ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SECURITY_METHOD_WAPI_SUB_ATTR_WAPIPREAUTH_UPDATE);*/
	dbus_error_init(&err);

	dbus_message_append_args(query,
						 DBUS_TYPE_BYTE,&security_id,
						 DBUS_TYPE_BYTE,&MulticaseRekeyStrict,
						 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);

	dbus_message_unref(query);

	if (NULL == reply)
	{
		if (dbus_error_is_set(&err)) 
		{
			dbus_error_free(&err);
		}
		return SNMPD_CONNECTION_ERROR;
	}

	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
		retu = 1;
	else if(ret == ASD_SECURITY_NOT_EXIST)	
		retu = -2;
	else if(ret == ASD_SECURITY_WLAN_SHOULD_BE_DISABLE) 	
		retu = -3;
	else if(ret == ASD_SECURITY_TYPE_NOT_MATCH_ENCRYPTION_TYPE) 
		retu = -4;
	else
		retu = -5;
	
	dbus_message_unref(reply);	
	return retu;
}

/*stateΪ"enable"��"disable"*/
int set_wapi_sub_attr_unicastcipherenabled_cmd(dbus_parameter parameter, DBusConnection *connection,int id,char *state)
																						   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾUnicastCipherEnabled should be enable or disable*/
																						   /*����-2��ʾsecurity profile does not exist������-3��ʾThis Security Profile be used by some Wlans,please disable them first*/
																						   /*����-4��ʾCan't set UnicastCipherEnabled under current security type������-5��ʾerror������-6��ʾSecurity ID�Ƿ�*/
																						   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{
	if(NULL == connection)
		return 0;
	
	if(NULL == state)
		return 0;
	
	int ret;
	unsigned char security_id;
	DBusMessage *query, *reply; 
	DBusMessageIter  iter;
	DBusError err;
	int retu;
	unsigned char UnicastCipherEnabled=0;

	//security_id = (int)vty->index;
	if ((!strcmp(state,"enable")))
		UnicastCipherEnabled = 1;			
	else if ((!strcmp(state,"disable")))
		UnicastCipherEnabled = 0;	
	else 
	{		
		return -1;
	}	

	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	security_id =  id;	
	if(security_id >= WLAN_NUM || security_id == 0){
		syslog(LOG_DEBUG,"security id in set_wapi_sub_attr_unicastcipherenabled_cmd is %d\n",security_id);
		return -6;
	}
	
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SECURITY_METHOD_WAPI_SUB_ATTR_UNICASTCIPHERENABLED_UPDATE);

	/*query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
					ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SECURITY_METHOD_WAPI_SUB_ATTR_WAPIPREAUTH_UPDATE);*/
	dbus_error_init(&err);

	dbus_message_append_args(query,
						 DBUS_TYPE_BYTE,&security_id,
						 DBUS_TYPE_BYTE,&UnicastCipherEnabled,
						 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);

	dbus_message_unref(query);

	if (NULL == reply)
	{
		if (dbus_error_is_set(&err)) 
		{
			dbus_error_free(&err);
		}
		return SNMPD_CONNECTION_ERROR;
	}

	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
		retu = 1;
	else if(ret == ASD_SECURITY_NOT_EXIST)
		retu = -2;
	else if(ret == ASD_SECURITY_WLAN_SHOULD_BE_DISABLE) 
		retu = -3;
	else if(ret == ASD_SECURITY_TYPE_NOT_MATCH_ENCRYPTION_TYPE) 	
		retu = -4;
	else
		retu = -5;
	
	dbus_message_unref(reply);	
	return retu;
}

/*δʹ��*/
/*stateΪ"enable"��"disable"*/
int set_wapi_sub_attr_authenticationsuiteenable_cmd(dbus_parameter parameter, DBusConnection *connection,int id,char *state)
																							   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾAuthenticationSuiteEnable should be enable or disable*/
																							   /*����-2��ʾsecurity profile does not exist������-3��ʾThis Security Profile be used by some Wlans,please disable them first*/
																							   /*����-4��ʾCan't set AuthenticationSuiteEnable under current security type������-5��ʾerror������-6��ʾSecurity ID�Ƿ�*/
																							   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{
	if(NULL == connection)
		return 0;
	
	if(NULL == state)
		return 0;
	
	int ret;
	unsigned char security_id;
	DBusMessage *query, *reply; 
	DBusMessageIter  iter;
	DBusError err;
	int retu;
	unsigned char AuthenticationSuiteEnable=0;

	//security_id = (int)vty->index;
	if ((!strcmp(state,"enable")))
		AuthenticationSuiteEnable = 1;			
	else if ((!strcmp(state,"disable")))
		AuthenticationSuiteEnable = 0;	
	else 
	{		
		return -1;
	}

	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	security_id = id;
	if(security_id >= WLAN_NUM || security_id == 0){
		syslog(LOG_DEBUG,"security id in set_wapi_sub_attr_authenticationsuiteenable_cmd is %d\n",security_id);
		return -6;
	}
	
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SECURITY_METHOD_WAPI_SUB_ATTR_AUTHENTICATIONSUITEENABLE_UPDATE);


	/*query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
					ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SECURITY_METHOD_WAPI_SUB_ATTR_WAPIPREAUTH_UPDATE);*/
	dbus_error_init(&err);

	dbus_message_append_args(query,
						 DBUS_TYPE_BYTE,&security_id,
						 DBUS_TYPE_BYTE,&AuthenticationSuiteEnable,
						 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);

	dbus_message_unref(query);

	if (NULL == reply)
	{
		if (dbus_error_is_set(&err)) 
		{
			dbus_error_free(&err);
		}
		return SNMPD_CONNECTION_ERROR;
	}

	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
		retu = 1;
	else if(ret == ASD_SECURITY_NOT_EXIST)	
		retu = -2;
	else if(ret == ASD_SECURITY_WLAN_SHOULD_BE_DISABLE) 	
		retu = -3;
	else if(ret == ASD_SECURITY_TYPE_NOT_MATCH_ENCRYPTION_TYPE) 		
		retu = -4;
	else
		retu = -5;
	
	dbus_message_unref(reply);	
	return retu;
}

/*value�ķ�Χ��0-64*/
int set_wapi_sub_attr_certificateupdatecount_cmd(dbus_parameter parameter, DBusConnection *connection,int id,char *value)
																							/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput retry value should be 0 to 64*/
																							/*����-2��ʾsecurity profile does not exist������-3��ʾThis Security Profile be used by some Wlans,please disable them first*/
																							/*����-4��ʾCan't set CertificateUpdateCount under current security type������-5��ʾerror������-6��ʾSecurity ID�Ƿ�*/
																							/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{
	if(NULL == connection)
		return 0;
	
	if(NULL == value)
		return 0;
	
	int ret;
	unsigned char security_id;
	DBusMessage *query, *reply; 
	DBusMessageIter  iter;
	DBusError err;
	int retu;
	int CertificateUpdateCount=0;

	//security_id = (int)vty->index;
	CertificateUpdateCount= atoi(value);

	if(CertificateUpdateCount<0||CertificateUpdateCount>64)
	{
		return -1;
	}

	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	security_id =  id;
	if(security_id >= WLAN_NUM || security_id == 0){
		syslog(LOG_DEBUG,"security id in set_wapi_sub_attr_certificateupdatecount_cmd is %d\n",security_id);
		return -6;
	}
	
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SECURITY_METHOD_WAPI_SUB_ATTR_CERTIFICATE_UPDATE);
	/*query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
					ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SECURITY_METHOD_WAPI_SUB_ATTR_CERTIFICATE_UPDATE);*/
	dbus_error_init(&err);

	dbus_message_append_args(query,
						 DBUS_TYPE_BYTE,&security_id,
						 DBUS_TYPE_UINT32,&CertificateUpdateCount,
						 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);

	dbus_message_unref(query);

	if (NULL == reply)
	{
		if (dbus_error_is_set(&err)) 
		{
			dbus_error_free(&err);
		}
		return SNMPD_CONNECTION_ERROR;
	}

	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
		retu = 1;
	else if(ret == ASD_SECURITY_NOT_EXIST)	
		retu = -2;
	else if(ret == ASD_SECURITY_WLAN_SHOULD_BE_DISABLE) 	
		retu = -3;
	else if(ret == ASD_SECURITY_TYPE_NOT_MATCH_ENCRYPTION_TYPE) 		
		retu = -4;
	else
		retu = -5;
	
	dbus_message_unref(reply);	
	return retu;
}

/*value�ķ�Χ��0-64*/
int set_wapi_sub_attr_multicastupdatecount_cmd(dbus_parameter parameter, DBusConnection *connection,int id,char *value)
																						   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput retry value should be 0 to 64*/
																						   /*����-2��ʾsecurity profile does not exist������-3��ʾThis Security Profile be used by some Wlans,please disable them first*/
																						   /*����-4��ʾCan't set MulticastUpdateCount under current security type������-5��ʾerror������-6��ʾSecurity ID�Ƿ�*/
																						   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{
	if(NULL == connection)
		return 0;
	
	if(NULL == value)
		return 0;
	
	int ret;
	unsigned char security_id;
	DBusMessage *query, *reply; 
	DBusMessageIter  iter;
	DBusError err;
	int retu;
	int MulticastUpdateCount=0;

	security_id = id;
	if(security_id >= WLAN_NUM || security_id == 0){
		syslog(LOG_DEBUG,"security id in set_wapi_sub_attr_multicastupdatecount_cmd is %d\n",security_id);
		return -6;
	}
	
	MulticastUpdateCount= atoi(value);

	if(MulticastUpdateCount<0||MulticastUpdateCount>64)
	{
		//vty_out(vty,"<error> input retry value should be 0 to 64.\n");
		return -1;
	}

	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SECURITY_METHOD_WAPI_SUB_ATTR_MULTICAST_UPDATE);

	/*query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
					ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SECURITY_METHOD_WAPI_SUB_ATTR_MULTICAST_UPDATE);*/
	dbus_error_init(&err);

	dbus_message_append_args(query,
						 DBUS_TYPE_BYTE,&security_id,
						 DBUS_TYPE_UINT32,&MulticastUpdateCount,
						 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);

	dbus_message_unref(query);

	if (NULL == reply)
	{
		if (dbus_error_is_set(&err)) 
		{
			dbus_error_free(&err);
		}
		return SNMPD_CONNECTION_ERROR;
	}

	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
		retu = 1;
	else if(ret == ASD_SECURITY_NOT_EXIST)		
		retu = -2;
	else if(ret == ASD_SECURITY_WLAN_SHOULD_BE_DISABLE) 	
		retu = -3;
	else if(ret == ASD_SECURITY_TYPE_NOT_MATCH_ENCRYPTION_TYPE) 	
		retu = -4;
	else
		retu = -5;
	
	dbus_message_unref(reply);	
	return retu;
}

/*value�ķ�Χ��0-64*/
int set_wapi_sub_attr_unicastupdatecount_cmd(dbus_parameter parameter, DBusConnection *connection,int id,char *value)
																						 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput  value should be 0 to 64*/
																						 /*����-2��ʾsecurity profile does not exist������-3��ʾThis Security Profile be used by some Wlans,please disable them first*/
																						 /*����-4��ʾCan't set UnicastUpdateCount under current security type������-5��ʾerror������-6��ʾSecurity ID�Ƿ�*/
																						 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{
	if(NULL == connection)
		return 0;
	
	if(NULL == value)
		return 0;
	
	int ret;
	unsigned char security_id;
	DBusMessage *query, *reply; 
	DBusMessageIter  iter;
	DBusError err;
	int retu;
	int UnicastUpdateCount=0;

	security_id = id;
	if(security_id >= WLAN_NUM || security_id == 0){
		syslog(LOG_DEBUG,"security id in set_wapi_sub_attr_unicastupdatecount_cmd is %d\n",security_id);
		return -6;
	}
	
	UnicastUpdateCount= atoi(value);

	if(UnicastUpdateCount<0||UnicastUpdateCount>64)
	{
		return -1;
	}

	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SECURITY_METHOD_WAPI_SUB_ATTR_UNICAST_COUNT_UPDATE);

	/*query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
					ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SECURITY_METHOD_WAPI_SUB_ATTR_UNICAST_COUNT_UPDATE);*/
	dbus_error_init(&err);

	dbus_message_append_args(query,
						 DBUS_TYPE_BYTE,&security_id,
						 DBUS_TYPE_UINT32,&UnicastUpdateCount,
						 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);

	dbus_message_unref(query);

	if (NULL == reply)
	{
		if (dbus_error_is_set(&err)) 
		{
			dbus_error_free(&err);
		}
		return SNMPD_CONNECTION_ERROR;
	}

	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
		retu = 1;
	else if(ret == ASD_SECURITY_NOT_EXIST)			
		retu = -2;
	else if(ret == ASD_SECURITY_WLAN_SHOULD_BE_DISABLE) 
		retu = -3;
	else if(ret == ASD_SECURITY_TYPE_NOT_MATCH_ENCRYPTION_TYPE) 		
		retu = -4;
	else
		retu = -5;
	
	dbus_message_unref(reply);	
	return retu;
}

/*value�ķ�Χ��0-86400*/
int set_wapi_sub_attr_bklifetime_cmd(dbus_parameter parameter, DBusConnection *connection,int id,char *value)
																			 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput value should be 0 to 86400*/
																			 /*����-2��ʾsecurity profile does not exist������-3��ʾThis Security Profile be used by some Wlans,please disable them first*/
																			 /*����-4��ʾCan't set BKLifetime under current security type������-5��ʾerror������-6��ʾSecurity ID�Ƿ�*/
																			 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{
	if(NULL == connection)
		return 0;
	
	if(NULL == value)
		return 0;
	
	int ret;
	unsigned char security_id;
	DBusMessage *query, *reply; 
	DBusMessageIter  iter;
	DBusError err;
	int retu;
	int BKLifetime=0;

	security_id = id;
	if(security_id >= WLAN_NUM || security_id == 0){
		syslog(LOG_DEBUG,"security id in set_wapi_sub_attr_bklifetime_cmd is %d\n",security_id);
		return -6;
	}
	
	BKLifetime= atoi(value);

	if(BKLifetime<0||BKLifetime>86400)
	{
		return -1;
	}

	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SECURITY_METHOD_WAPI_SUB_ATTR_BKLIFETIME_UPDATE);

	/*query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
					ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SECURITY_METHOD_WAPI_SUB_ATTR_BKLIFETIME_UPDATE);*/
	dbus_error_init(&err);

	dbus_message_append_args(query,
						 DBUS_TYPE_BYTE,&security_id,
						 DBUS_TYPE_UINT32,&BKLifetime,
						 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);

	dbus_message_unref(query);

	if (NULL == reply)
	{
		if (dbus_error_is_set(&err)) 
		{
			dbus_error_free(&err);
		}
		return SNMPD_CONNECTION_ERROR;
	}

	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
		retu = -1;
	else if(ret == ASD_SECURITY_NOT_EXIST)			
		retu = -2;
	else if(ret == ASD_SECURITY_WLAN_SHOULD_BE_DISABLE) 	
		retu = -3;
	else if(ret == ASD_SECURITY_TYPE_NOT_MATCH_ENCRYPTION_TYPE) 	
		retu = -4;
	else
		retu = -5;
	
	dbus_message_unref(reply);	
	return retu;
}

/*value�ķ�Χ��0-99*/
int set_wapi_sub_attr_bkreauththreshold_cmd(dbus_parameter parameter, DBusConnection *connection,int id,char *value)
																					   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput  value should be 0 to 99*/
																					   /*����-2��ʾsecurity profile does not exist������-3��ʾThis Security Profile be used by some Wlans,please disable them first*/
																					   /*����-4��ʾCan't set BKReauthThreshold under current security type������-5��ʾerror������-6��ʾSecurity ID�Ƿ�*/
																					   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{
	if(NULL == connection)
		return 0;
	
	if(NULL == value)
		return 0;
	
	int ret;
	unsigned char security_id;
	DBusMessage *query, *reply; 
	DBusMessageIter  iter;
	DBusError err;
	int retu;
	int BKReauthThreshold=0;

	//security_id = (int)vty->index;
	BKReauthThreshold= atoi(value);

	if(BKReauthThreshold<0||BKReauthThreshold>99)
	{
		return -1;
	}

	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	security_id = id;	
	if(security_id >= WLAN_NUM || security_id == 0){
		syslog(LOG_DEBUG,"security id in set_wapi_sub_attr_bkreauththreshold_cmd is %d\n",security_id);
		return -6;
	}
	
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SECURITY_METHOD_WAPI_SUB_ATTR_BKREAUTH_THREASHOLD_UPDATE);

	/*query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
					ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SECURITY_METHOD_WAPI_SUB_ATTR_BKREAUTH_THREASHOLD_UPDATE);*/
	dbus_error_init(&err);

	dbus_message_append_args(query,
						 DBUS_TYPE_BYTE,&security_id,
						 DBUS_TYPE_UINT32,&BKReauthThreshold,
						 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);

	dbus_message_unref(query);

	if (NULL == reply)
	{
		if (dbus_error_is_set(&err)) 
		{
			dbus_error_free(&err);
		}
		return SNMPD_CONNECTION_ERROR;
	}

	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
		retu = 1;
	else if(ret == ASD_SECURITY_NOT_EXIST)			
		retu = -2;
	else if(ret == ASD_SECURITY_WLAN_SHOULD_BE_DISABLE) 		
		retu = -3;
	else if(ret == ASD_SECURITY_TYPE_NOT_MATCH_ENCRYPTION_TYPE) 	
		retu = -4;
	else
		retu = -5;
	
	dbus_message_unref(reply);	
	return retu;
}

/*value�ķ�Χ��0-120*/
int set_wapi_sub_attr_satimeout_cmd(dbus_parameter parameter, DBusConnection *connection,int id,char *value)
																			  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput retry value should be 0 to 120*/
																			  /*����-2��ʾsecurity profile does not exist������-3��ʾThis Security Profile be used by some Wlans,please disable them first*/
																			  /*����-4��ʾCan't set SATimeout under current security type������-5��ʾerror������-6��ʾSecurity ID�Ƿ�*/
																			  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{
	if(NULL == connection)
		return 0;
	
	if(NULL == value)
		return 0;
	
	int ret;
	unsigned char security_id;
	DBusMessage *query, *reply; 
	DBusMessageIter  iter;
	DBusError err;
	int retu;
	int SATimeout=0;

	//security_id = (int)vty->index;
	SATimeout= atoi(value);

	if(SATimeout<0||SATimeout>120)
	{
		return -1;
	}

	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	security_id = id;	
	if(security_id >= WLAN_NUM || security_id == 0){
		syslog(LOG_DEBUG,"security id in set_wapi_sub_attr_satimeout_cmd is %d\n",security_id);
		return -6;
	}
	
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SECURITY_METHOD_WAPI_SUB_ATTR_SA_TIMEOUT_UPDATE);

	/*query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
					ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SECURITY_METHOD_WAPI_SUB_ATTR_SA_TIMEOUT_UPDATE);*/
	dbus_error_init(&err);

	dbus_message_append_args(query,
						 DBUS_TYPE_BYTE,&security_id,
						 DBUS_TYPE_UINT32,&SATimeout,
						 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);

	dbus_message_unref(query);

	if (NULL == reply)
	{
		if (dbus_error_is_set(&err)) 
		{
			dbus_error_free(&err);
		}
		return SNMPD_CONNECTION_ERROR;
	}

	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
		retu = 1;
	else if(ret == ASD_SECURITY_NOT_EXIST)			
		retu = -2;
	else if(ret == ASD_SECURITY_WLAN_SHOULD_BE_DISABLE) 	
		retu = -3;
	else if(ret == ASD_SECURITY_TYPE_NOT_MATCH_ENCRYPTION_TYPE) 		
		retu = -4;
	else
		retu = -5;
	
	dbus_message_unref(reply);	
	return retu;
}


void Free_security_wapi_info(struct dcli_security *sec)
{
	void (*dcli_init_free_func)(struct dcli_security *);
	if(NULL != ccgi_dl_handle)
	{
		dcli_init_free_func = dlsym(ccgi_dl_handle,"dcli_free_security");
		if(NULL != dcli_init_free_func)
		{
			dcli_init_free_func(sec);
		}
	}
}

/*δʹ��*/
/*ֻҪ���ã���ͨ��Free_security_wapi_info()�ͷſռ�*/
int  show_security_wapi_info_cmd(dbus_parameter parameter, DBusConnection *connection,char *id,struct dcli_security **sec)
												/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
												/*����-2��ʾsecurity id should be 1 to WLAN_NUM-1������-3��ʾsecurity id does not exist*/
												/*����-4��ʾsecurity id should be wapi_psk or wapi_auth*/
												/*����-5��ʾsecurity's wapi config  is not intergrity������-6��ʾerror*/
												/*����-7��ʾillegal input:Input exceeds the maximum value of the parameter type*/
												/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{
	if(NULL == connection)
		return 0;

	if(NULL == id)
	{
		*sec = NULL;
		return 0;
	}
	
	unsigned char 	security_id;		
	int ret;
	int retu;

	ret = parse_char_ID((char*)id, &security_id);
	
	if(ret != WID_DBUS_SUCCESS){
        if(ret == WID_ILLEGAL_INPUT){
			retu = -7;
        }
		else{
			retu = -1;
		}
		return retu;
	}	
	
	if(security_id >= WLAN_NUM || security_id == 0){
		return -2;
	}

	void*(*dcli_init_func)(
					DBusConnection *,
					int , 
					unsigned char , 
					int ,
					unsigned int *
					);

	*sec = NULL;
	if(NULL != ccgi_dl_handle)
	{
		dcli_init_func = dlsym(ccgi_dl_handle,"show_security_wapi_info");
		if(NULL != dcli_init_func)
		{
			*sec = (*dcli_init_func)
				(
					connection, 
					parameter.instance_id, 
					security_id, 
					parameter.local_id, 
					&ret
				);
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
						
	if((ret == 0 ) && (sec != NULL))
		retu = 1;
	else if (ret == ASD_SECURITY_NOT_EXIST)
		retu = -3;
	else if (ret == ASD_SECURITY_TYPE_WITHOUT_WAPI_AUTH)
		retu = -4;
	else if (ret == ASD_SECURITY_PROFILE_NOT_INTEGRITY)
		retu = -5;
	else if (ret == ASD_DBUS_ERROR)		
		retu = SNMPD_CONNECTION_ERROR;
	else
		retu = -6;
	
	return retu;		
}

/*δʹ��*/
/*stateΪ"enable"��"disable"*/
int  config_asd_get_sta_info_able_cmd(dbus_parameter parameter, DBusConnection *connection,char *state)/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾparameter illegal������-2��ʾerror*/
																											  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{	
	if(NULL == connection)
		return 0;
	
	if(NULL == state)
		return 0;
	
	int ret = ASD_DBUS_SUCCESS;
	unsigned char type;
	DBusMessage *query, *reply; 
	DBusMessageIter  iter;
	DBusError err;
	int retu;
	
	if ((!strcmp(state,"enable")))
		type = 1;			
	else if ((!strcmp(state,"disable")))
		type = 0;	
	else {		
		return -1;
	}

	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SET_ASD_GET_STA_INFO_ABLE);
	
	/*query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
						ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SET_ASD_GET_STA_INFO_ABLE);*/
	dbus_error_init(&err);

	dbus_message_append_args(query,
							 DBUS_TYPE_BYTE,&type,
							 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);
	
	dbus_message_unref(query);
	
	if (NULL == reply) {
		if (dbus_error_is_set(&err)) {
			dbus_error_free(&err);
		}
		return SNMPD_CONNECTION_ERROR;
	}
	
	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
		retu = 1;
	else
		retu = -2;
	
	dbus_message_unref(reply);
	return retu;
}

/*δʹ��*/
/*value�ķ�Χ��5-3600*/
int config_asd_get_sta_info_time_cmd(dbus_parameter parameter, DBusConnection *connection,char *value)/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput time value should be 5 to 3600������-2��ʾerror*/
																											 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{
	if(NULL == connection)
		return 0;
	
	if(NULL == value)
		return 0;
	
	int ret;
	DBusMessage *query, *reply; 
	DBusMessageIter  iter;
	DBusError err;
	int retu;
	int period=20;

	period= atoi(value);

	if(period<5 || period>3600){
		return -1;
	}

	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SET_ASD_GET_STA_INFO_TIME);

	/*query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
					ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SET_ASD_GET_STA_INFO_TIME); */
	dbus_error_init(&err);

	dbus_message_append_args(query,
						 DBUS_TYPE_UINT32,&period,
						 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);

	dbus_message_unref(query);

	if (NULL == reply)	{
		if (dbus_error_is_set(&err)) {
			dbus_error_free(&err);
		}
		return SNMPD_CONNECTION_ERROR;
	}

	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
		retu = 1;
	else
		retu = -2;
	
	dbus_message_unref(reply);	
	return retu;
}

/*δʹ��*/
/*stateΪ"enable"��"disable"*/
int set_notice_sta_info_to_portal_open_cmd(dbus_parameter parameter, DBusConnection *connection,char *state)/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput should be enable or disable������-2��ʾerror*/
																												   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{
	if(NULL == connection)
		return 0;
	
	if(NULL == state)
		return 0;
	
	int ret;
	DBusMessage *query, *reply; 
	DBusMessageIter  iter;
	DBusError err;
	int retu;
	
	unsigned char asd_send_notice_sta_info_to_proto=0;
	if(!strcmp(state,"enable")){
		asd_send_notice_sta_info_to_proto=1;
	}
	else if(!strcmp(state,"disable")){
		asd_send_notice_sta_info_to_proto=0;
	}
	else 
	{
		return -1;
	}


	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	/*query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
					ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_NOTICE_STA_INFO_TO_PROTO_OPEN);*/
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_NOTICE_STA_INFO_TO_PROTO_OPEN);
	dbus_error_init(&err);

	dbus_message_append_args(query,
						 DBUS_TYPE_BYTE,&asd_send_notice_sta_info_to_proto,
						 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);

	dbus_message_unref(query);

	if (NULL == reply)
	{
		if (dbus_error_is_set(&err)) 
		{
			dbus_error_free(&err);
		}
		return SNMPD_CONNECTION_ERROR;
	}

	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
		retu = 1;
	else
		retu = -2;
	
	dbus_message_unref(reply);	
	return retu;
}

/*stateΪ"enable"��"disable"*/
int config_wapi_multi_cert_cmd(dbus_parameter parameter, DBusConnection *connection,int SID,char *state)
																		/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾparameter illegal*/
																		/*����-2��ʾsecurity type which you chose does not support wapi authentication*/
																		/*����-3��ʾthis security profile be used by some wlans,please disable them first*/
																		/*����-4��ʾerror������-5��ʾSecurity ID�Ƿ�*/
																		/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{	
	if(NULL == connection)
		return 0;
	
	if(NULL == state)
		return 0;
	
	int ret = ASD_DBUS_SUCCESS;
	unsigned char security_id;
	unsigned char type;
	DBusMessage *query, *reply; 
	DBusMessageIter  iter;
	DBusError err;
	int retu;
	
	if ((!strcmp(state,"enable")))
		type = 1;			
	else if ((!strcmp(state,"disable")))
		type = 0;	
	else {		
		return -1;
	}
	

	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	security_id = SID;	
	if(security_id >= WLAN_NUM || security_id == 0){
		syslog(LOG_DEBUG,"security id in config_wapi_multi_cert_cmd is %d\n",security_id);
		return -5;
	}
	
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SECURITY_METHOD_SET_WAPI_MULTI_CERT);
	/*query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
						ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SECURITY_METHOD_SET_WAPI_MULTI_CERT);*/
	dbus_error_init(&err);

	dbus_message_append_args(query,
							DBUS_TYPE_BYTE,&security_id,
							 DBUS_TYPE_BYTE,&type,
							 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);
	
	dbus_message_unref(query);
	
	if (NULL == reply) {
		if (dbus_error_is_set(&err)) {
			dbus_error_free(&err);
		}
		return SNMPD_CONNECTION_ERROR;
	}
	
	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
		retu = 1;
	else if(ret == ASD_SECURITY_TYPE_WITHOUT_WAPI_AUTH)			
		retu = -2;
	else if(ret == ASD_SECURITY_WLAN_SHOULD_BE_DISABLE)		
		retu = -3;
	else
		retu = -4;
	
	dbus_message_unref(reply);
	return retu;
}

void Free_wlanid_security_wapi_info(struct dcli_security *sec)
{
	void (*dcli_init_free_func)(struct dcli_security *);
	if(NULL != ccgi_dl_handle)
	{
		dcli_init_free_func = dlsym(ccgi_dl_handle,"dcli_free_security");
		if(NULL != dcli_init_free_func)
		{
			dcli_init_free_func(sec);
		}
	}
}

/*ֻҪ���ã���ͨ��Free_wlanid_security_wapi_info()�ͷſռ�*/
int show_wlanid_security_wapi_config_cmd(dbus_parameter parameter, DBusConnection *connection,char *wlanID,struct dcli_security	**sec)
																										  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
																									      /*����-2��ʾwlan id should be 1 to WLAN_NUM-1������-3��ʾwlan id does not exist*/
																									      /*����-4��ʾsecurity id should be wapi_psk or wapi_auth*/
																									      /*����-5��ʾsecurity's wapi config  is not intergrity������-6 ��ʾwlan has not apply any security*/
																									      /*����-7��ʾerror������-8��ʾillegal input:Input exceeds the maximum value of the parameter type*/
																										  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{
	if(NULL == connection)
		return 0;

	if(NULL == wlanID)
	{
		*sec = NULL;
		return 0;
	}
	
	int ret;
	int retu;
	unsigned char wlan_id;

	ret = parse_char_ID((char*)wlanID, &wlan_id);
	
	if(ret != WID_DBUS_SUCCESS){
        if(ret == WID_ILLEGAL_INPUT){
			retu = -8;
        }
		else{
			retu = -1;
		}
		return retu;
	}	
	if(wlan_id >= WLAN_NUM || wlan_id == 0){
		return -2;
	}

	void*(*dcli_init_func)(
					DBusConnection *,
					int , 
					unsigned char , 
					int ,
					unsigned int *
					);

	*sec = NULL;
	if(NULL != ccgi_dl_handle)
	{
		dcli_init_func = dlsym(ccgi_dl_handle,"show_wlan_security_wapi_info");
		if(NULL != dcli_init_func)
		{
			*sec = (*dcli_init_func)
				(
					connection, 
					parameter.instance_id, 
					wlan_id, 
					parameter.local_id, 
					&ret
				);
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}

	if((ret == 0) && (sec != NULL))
		retu = 1;	
	else if (ret == ASD_WLAN_NOT_EXIST)
		retu = -3;
	else if (ret == ASD_SECURITY_TYPE_WITHOUT_WAPI_AUTH)
		retu = -4;
	else if (ret == ASD_SECURITY_PROFILE_NOT_INTEGRITY)
		retu = -5;
	else if (ret == ASD_SECURITY_NOT_EXIST)
		retu = -6;
	else if (ret == ASD_DBUS_ERROR)		
		retu = SNMPD_CONNECTION_ERROR;
	else
		retu = -7;	
	
	return retu;			
}

void Free_radius_cmd(struct dcli_security *sec)
{
	void (*dcli_init_free_func)(struct dcli_security *);
	if(NULL != ccgi_dl_handle)
	{
		dcli_init_free_func = dlsym(ccgi_dl_handle,"dcli_free_security");
		if(NULL != dcli_init_free_func)
		{
			dcli_init_free_func(sec);
		}
	}
}

/*����1ʱ������Free_radius_cmd()�ͷſռ�*/
int show_radius_cmd(dbus_parameter parameter, DBusConnection *connection,char *Radius_ID,struct dcli_security **sec)
																					/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																					/*����-1��ʾunknown id format*/
																					/*����-2��ʾradius id should be 1 to WLAN_NUM-1*/
																					/*����-3��ʾradius does not exist������-4��ʾerror*/
																					/*����-5��ʾillegal input:Input exceeds the maximum value of the parameter type*/
																					/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{	
	if(NULL == connection)
		return 0;

	if(NULL == Radius_ID)
	{
		*sec = NULL;
		return 0;
	}
	
	unsigned int 	ret = 0;
	unsigned char 	radius_id = 0;
	int retu;

	ret = parse_char_ID((char*)Radius_ID, &radius_id);
	if(ret != WID_DBUS_SUCCESS){
        if(ret == WID_ILLEGAL_INPUT){
			retu = -5;
        }
		else{
			retu = -1;
		}
		return retu;
	}	
	if(radius_id >= WLAN_NUM || radius_id == 0){
		return -2;
	}


	void*(*dcli_init_func)(
					DBusConnection *,
					int , 
					unsigned char , 
					int ,
					unsigned int *
					);

	*sec = NULL;
	if(NULL != ccgi_dl_handle)
	{
		dcli_init_func = dlsym(ccgi_dl_handle,"show_radius_id");
		if(NULL != dcli_init_func)
		{
			*sec = (*dcli_init_func)
				(
					connection, 
					parameter.instance_id, 
					radius_id, 
					parameter.local_id, 
					&ret
				);
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}

	if ((ret == 0) && (sec != NULL)) 
		retu = 1;
	else if (ret == ASD_SECURITY_NOT_EXIST)
		retu = -3;
	else if (ret == ASD_DBUS_ERROR)	
		retu = SNMPD_CONNECTION_ERROR;
	else
		retu = -4;
		
	return retu;	
}

/*value�ķ�Χ��1-255*/
int set_wapi_sub_attr_multicast_cipher_cmd(dbus_parameter parameter, DBusConnection *connection,int sid,char *value)
																					  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput retry value should be 1 to 255*/
																					  /*����-2��ʾsecurity profile does not exist������-3��ʾThis Security Profile be used by some Wlans,please disable them first*/
																					  /*����-4��ʾCan't set multicast sipher under current security type������-5��ʾerror������-6��ʾSecurity ID�Ƿ�*/
																					  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{
	if(NULL == connection)
		return 0;
	
	if(NULL == value)
		return 0;
	
	int ret;
	unsigned char security_id;
	DBusMessage *query, *reply; 
	DBusMessageIter  iter;
	DBusError err;
	unsigned char type = 0;
	int retu;

	type = atoi(value);

	if(type<1 || type>255)
	{
		return -1;
	}	

	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	security_id = sid;	
	if(security_id >= WLAN_NUM || security_id == 0){
		syslog(LOG_DEBUG,"security id in set_wapi_sub_attr_multicast_cipher_cmd is %d\n",security_id);
		return -6;
	}
	
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SECURITY_METHOD_WAPI_SUB_ATTR_MULTICAST_CIPHER);

	
	dbus_error_init(&err);

	dbus_message_append_args(query,
						 DBUS_TYPE_BYTE,&security_id,
						 DBUS_TYPE_BYTE,&type,
						 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);

	dbus_message_unref(query);

	if (NULL == reply)
	{
		if (dbus_error_is_set(&err)) 
		{
			dbus_error_free(&err);
		}
		return SNMPD_CONNECTION_ERROR;
	}

	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
		retu = 1;
	else if(ret == ASD_SECURITY_NOT_EXIST)			
		retu = -2;
	else if(ret == ASD_SECURITY_WLAN_SHOULD_BE_DISABLE) 	
		retu = -3;
	else if(ret == ASD_SECURITY_TYPE_NOT_MATCH_ENCRYPTION_TYPE) 
		retu = -4;
	else
		retu = -5;
	
	dbus_message_unref(reply);	
	return retu;
}


/*δʹ��*/
/*stateΪ"enable"��"disable"*/
int config_asd_ieee_80211n_able_cmd(dbus_parameter parameter, DBusConnection *connection,char *state)
																		/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																	    /*����-1��ʾparameter illegal������-2��ʾerror*/
																		/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{	
	if(NULL == connection)
		return 0;
	
	if(NULL == state)
		return 0;
	
	int ret = ASD_DBUS_SUCCESS;
	unsigned char type;
	DBusMessage *query, *reply; 
	DBusMessageIter  iter;
	DBusError err;
	int retu;
	
	if ((!strcmp(state,"enable")))
		type = 1;			
	else if ((!strcmp(state,"disable")))
		type = 0;	
	else {		
		return -1;
	}

	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SET_ASD_PROCESS_80211N_ABLE);
	
	dbus_error_init(&err);

	dbus_message_append_args(query,
							 DBUS_TYPE_BYTE,&type,
							 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);
	
	dbus_message_unref(query);
	
	if (NULL == reply) {
		if (dbus_error_is_set(&err)) {
			dbus_error_free(&err);
		}
		return SNMPD_CONNECTION_ERROR;
	}
	
	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
		retu = 1;
	else
		retu = -2;
	
	dbus_message_unref(reply);
	return retu;
}


/*sec_index�ķ�Χ��1-4*/
int set_security_wep_index_cmd(dbus_parameter parameter, DBusConnection *connection,int sid,char *sec_index)
																			 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format������-2��ʾinput security index should be 1 to 4*/
																			 /*����-3��ʾsecurity profile does not exist������-4��ʾThis Security Profile be used by some Wlans,please disable them first*/
																			 /*����-5��ʾthe encryption type of the security should be wep������-6��ʾerror������-7��ʾSecurity ID�Ƿ�*/
																			 /*����-8��ʾillegal input:Input exceeds the maximum value of the parameter type*/
																			 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{
	if(NULL == connection)
		return 0;
	
	if(NULL == sec_index)
		return 0;
	
	int ret,ret1;
	unsigned char security_id;
	DBusMessage *query, *reply; 
	DBusMessageIter  iter;
	DBusError err;
	int retu;	
	unsigned char security_index=0;

	//security_id = (int)vty->index;
	
	ret1 = parse_char_ID((char*)sec_index, &security_index);
	if(ret1 != WID_DBUS_SUCCESS){
            if(ret1 == WID_ILLEGAL_INPUT){
				retu = -8;
            }
			else{
				retu = -1;
			}
			return retu;
	}
	//security_index= atoi(sec_index);

	if(security_index<1||security_index>4)
	{
		return -2;
	}

	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];
	

	security_id = sid;	
	if(security_id >= WLAN_NUM || security_id == 0){
		syslog(LOG_DEBUG,"security id in set_security_wep_index_cmd is %d\n",security_id);
		return -7;
	}
			
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SECURITY_METHOD_WEP_INDEX_PERIOD);

	dbus_error_init(&err);

	dbus_message_append_args(query,
						 DBUS_TYPE_BYTE,&security_id,
						 DBUS_TYPE_BYTE,&security_index,
						 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);

	dbus_message_unref(query);

	if (NULL == reply)
	{
		if (dbus_error_is_set(&err)) 
			{
				dbus_error_free(&err);
			}
		return SNMPD_CONNECTION_ERROR;
	}
	
	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
		retu = 1;
	else if(ret == ASD_SECURITY_NOT_EXIST)	
		retu = -3;
	else if(ret == ASD_SECURITY_WLAN_SHOULD_BE_DISABLE) 	
		retu = -4;
 	else if(ret == ASD_SECURITY_TYPE_NOT_MATCH_ENCRYPTION_TYPE) 		
		retu = -5;
	else
		retu = -6;
	
	dbus_message_unref(reply);			
	return retu;
}

/*δʹ��*/
/*trap_typeΪ"wtp_deny_sta","sta_verify_failed","sta_assoc_failed","wapi_invalid_cert","wapi_challenge_replay","wapi_mic_juggle","wapi_low_safe","wapi_addr_redirection"*/
/*stateΪ"enable"��"disable"*/
int config_set_asd_trap_able_cmd(dbus_parameter parameter, DBusConnection *connection,char *trap_type,char *state)/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾparameter illegal������-2��ʾerror*/
																													    /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{	
	if(NULL == connection)
		return 0;
	
	if((NULL == trap_type)||(NULL == state))
		return 0;
	
	int ret = ASD_DBUS_SUCCESS;
	unsigned char type = 0;
	unsigned char flag = 0;
	DBusMessage *query, *reply; 
	DBusMessageIter  iter;
	DBusError err;
	int retu;
	
	if ((!strcmp(trap_type,"wtp_deny_sta")))
		flag = 1;			
	else if ((!strcmp(trap_type,"sta_verify_failed")))
		flag = 2;	
	else if ((!strcmp(trap_type,"sta_assoc_failed")))
		flag = 3;	
	else if ((!strcmp(trap_type,"wapi_invalid_cert")))
		flag = 4;	
	else if ((!strcmp(trap_type,"wapi_challenge_replay")))
		flag = 5;	
	else if ((!strcmp(trap_type,"wapi_mic_juggle")))
		flag = 6;	
	else if ((!strcmp(trap_type,"wapi_low_safe")))
		flag = 7;	
	else if ((!strcmp(trap_type,"wapi_addr_redirection")))
		flag = 8;	
	else {		
		return -1;
	}

	if ((!strcmp(state,"enable")))
		type = 1;			
	else if ((!strcmp(state,"disable")))
		type = 0;	
	else {		
		return -1;
	}	

	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SET_TRAP_ABLE);
	/*query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
						ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SET_TRAP_ABLE);*/
	dbus_error_init(&err);

	dbus_message_append_args(query,
							 DBUS_TYPE_BYTE,&flag,
							 DBUS_TYPE_BYTE,&type,
							 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);
	
	dbus_message_unref(query);
	
	if (NULL == reply) {
		if (dbus_error_is_set(&err)) {
			dbus_error_free(&err);
		}
		return SNMPD_CONNECTION_ERROR;
	}
	
	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
		retu = 1;
	else
		retu = -2;
	
	dbus_message_unref(reply);
	return retu;
}

/*δʹ��*/
int show_asd_trap_state_cmd(dbus_parameter parameter, DBusConnection *connection,struct asd_trap_state_info *info)/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																														/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{	
	if(NULL == connection)
		return 0;
	
	if(NULL == info)
		return 0;
	
	int ret = ASD_DBUS_SUCCESS;
	unsigned char type[8] = {0};
	DBusMessage *query, *reply; 
	DBusMessageIter  iter;
	DBusError err;
	int retu;	

	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ccgi_ReInitDbusPath_v2(parameter.local_id, parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SHOW_TRAP_STATE);
	/*query = dbus_message_new_method_call(ASD_DBUS_BUSNAME,ASD_DBUS_SECURITY_OBJPATH,\
						ASD_DBUS_SECURITY_INTERFACE,ASD_DBUS_SHOW_TRAP_STATE);*/
	dbus_error_init(&err);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);
	
	dbus_message_unref(query);
	
	if (NULL == reply) {
		if (dbus_error_is_set(&err)) {
			dbus_error_free(&err);
		}
		return SNMPD_CONNECTION_ERROR;
	}
	
	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);
	
	dbus_message_iter_next(&iter);
	dbus_message_iter_get_basic(&iter,&(type[0]));	

	dbus_message_iter_next(&iter);
	dbus_message_iter_get_basic(&iter,&(type[1]));	

	dbus_message_iter_next(&iter);
	dbus_message_iter_get_basic(&iter,&(type[2]));	

	dbus_message_iter_next(&iter);
	dbus_message_iter_get_basic(&iter,&(type[3]));	

	dbus_message_iter_next(&iter);
	dbus_message_iter_get_basic(&iter,&(type[4]));	

	dbus_message_iter_next(&iter);
	dbus_message_iter_get_basic(&iter,&(type[5]));	

	dbus_message_iter_next(&iter);
	dbus_message_iter_get_basic(&iter,&(type[6]));	

	dbus_message_iter_next(&iter);
	dbus_message_iter_get_basic(&iter,&(type[7]));	

	if(ret == ASD_DBUS_SUCCESS){
		memset(info->type,0,8);
		info->type[0] = type[0];
		info->type[1] = type[1];
		info->type[2] = type[2];
		info->type[3] = type[3];
		info->type[4] = type[4];
		info->type[5] = type[5];
		info->type[6] = type[6];
		info->type[7] = type[7];

		retu = 1;
	}
	else
	{
		retu = -1;
	}
	
	dbus_message_unref(reply);
	return retu;
}


/*cer_typeΪ"as","ae"��"ca"*/
int config_wapi_p12_cert_auth_path_cmd(dbus_parameter parameter, DBusConnection *connection,int sid,char *cer_type,char *path,char *passwd)
																											  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																											  /*����-1��ʾcertification isn't exit or can't be read*/
																											  /*����-2��ʾsecurity type which you chose does not support wapi authentication*/
																											  /*����-3��ʾthis security profile be used by some wlans,please disable them first*/
																											  /*����-4��ʾthis security profile isn't integrity*/
																											  /*����-5��ʾp12 cert password error������-6��ʾerror*/
																											  /*����-7��ʾSecurity ID�Ƿ�*/
{	
	if(NULL == connection)
		return 0;

	if((NULL == cer_type)||(NULL == path)||(NULL == passwd))
		return 0;
	
	unsigned char security_id;
	unsigned int type;
	char *cert_path;
	char *pass_word;
	int   ret;
	int retu;
	
	if (!strcmp(cer_type,"as"))				   /*as--1 ; ae--2; ca--3*/
		type = 1;			
	else if (!strcmp(cer_type,"ae"))
		type = 2;
	else if (!strcmp(cer_type,"ca"))
		type = 3;

	
	if(access(path,R_OK) != 0){
		return -1;
	}
	cert_path = (char *)malloc(strlen(path)+1);
	if(NULL == cert_path)
		return 0;
	memset(cert_path, 0, strlen(path)+1);
	memcpy(cert_path, path, strlen(path));
	
	pass_word = (char *)malloc(strlen(passwd)+1);
	if(NULL == pass_word)
		return 0;
	memset(pass_word, 0, strlen(passwd)+1);
	memcpy(pass_word, passwd, strlen(passwd));


	security_id = sid;	
	if(security_id >= WLAN_NUM || security_id == 0){
		syslog(LOG_DEBUG,"security id in config_wapi_p12_cert_auth_path_cmd is %d\n",security_id);
		return -7;
	}

	void(*dcli_init_func)(
							DBusConnection *, 
							int , 
							unsigned char ,
							unsigned int , 
							unsigned char *,  
							unsigned char *, 
							int ,
							unsigned int *
						);

	if(NULL != ccgi_dl_handle)
	{
		dcli_init_func = dlsym(ccgi_dl_handle,"set_wapi_p12_cert_path");
		if(NULL != dcli_init_func)
		{
			(*dcli_init_func)
				  (
					 connection, 
					 parameter.instance_id, 
					 security_id, 
					 type, 
					 cert_path, 
					 pass_word, 
					 parameter.local_id, 
					 &ret
				  );
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}

	if(ret == ASD_DBUS_SUCCESS)
		retu = 1;
	else if(ret == ASD_SECURITY_TYPE_WITHOUT_WAPI_AUTH) 		
		retu = -2;
	else if(ret == ASD_SECURITY_WLAN_SHOULD_BE_DISABLE) 		
		retu = -3;
	else if(ret == ASD_SECURITY_PROFILE_NOT_INTEGRITY)
		retu = -4;
	else if(ret == ASD_DBUS_ERROR)
		retu = -5;
	else
		retu = -6;
		
	return retu;
}


void Free_show_radius_all_cmd(struct dcli_security *sec)
{
	void (*dcli_init_free_func)(struct dcli_security *);
	if(NULL != ccgi_dl_handle)
	{
		dcli_init_free_func = dlsym(ccgi_dl_handle,"dcli_free_security_list");
		if(NULL != dcli_init_free_func)
		{
			dcli_init_free_func(sec);
		}
	}
}

/*����1ʱ������Free_show_radius_all_cmd()�ͷſռ�*/
int show_radius_all_cmd(dbus_parameter parameter, DBusConnection *connection,struct dcli_security **sec)
																		/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																		/*����-1��ʾno security support radius*/
																		/*����-2��ʾerror*/
																		/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
{	
	
	if(NULL == connection)
		return 0;

	unsigned int 	ret = 0;
	unsigned char 	num = 0;
	int retu = 0;

	void *(*dcli_init_func)(
							DBusConnection *,
							int , 
							unsigned char *, 
							int ,
							unsigned int *
						);

	*sec = NULL;
	if(NULL != ccgi_dl_handle)
	{
		dcli_init_func = dlsym(ccgi_dl_handle,"show_radius_all");
		if(NULL != dcli_init_func)
		{
			*sec = (*dcli_init_func)
				  (
					 connection, 
					 parameter.instance_id, 
					 &num, 
					 parameter.local_id, 
					 &ret
				  );
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
	
	if ((ret == 0)&&(*sec))
	{
		retu = 1;
	}	
	else if (ret == ASD_DBUS_ERROR)
	{
		return SNMPD_CONNECTION_ERROR;
	}
	else if (ret == ASD_SECURITY_NOT_EXIST)
	{
		retu = -1;
	}
	else
	{
		retu = -2;
	}
		
	return retu;
}


/*stateΪ"enable"��"disable"*/
int config_hybrid_auth_cmd(dbus_parameter parameter, DBusConnection *connection,int id,char *state)
																				 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown security type������-2��ʾSecurity ID�Ƿ�*/
																				 /*����-3��ʾsecurity profile does not exist������-4��ʾthis security profile is used by some wlans,please disable them first*/
																				 /*����-5��ʾerror������SNMPD_CONNECTION_ERROR��ʾconnection error*/
{	
	if(NULL == connection)
		return 0;
	
	if(NULL == state)
		return 0;

	int ret = ASD_DBUS_SUCCESS;
	unsigned char security_id = 0;
	unsigned int type = 0;
	DBusMessage *query, *reply; 
	DBusMessageIter  iter;
	DBusError err;
	int retu = 0;	
	
	if ((!strcmp(state,"enable")))
		type = 1;			
	else if ((!strcmp(state,"disable")))
		type = 0;	
	else 
	{		
		return -1;
	}

	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	security_id = id;
	if(security_id >= WLAN_NUM || security_id == 0){
		syslog(LOG_DEBUG,"security id in config_hybrid_auth_cmd is %d\n",security_id);
		return -2;
	}

	ReInitDbusPath_V2(parameter.local_id,parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ReInitDbusPath_V2(parameter.local_id,parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ReInitDbusPath_V2(parameter.local_id,parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);

	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_SECURITY_METHOD_HYBRID_AUTH);

	dbus_error_init(&err);
	dbus_message_append_args(query,
							 DBUS_TYPE_BYTE,&security_id,
							 DBUS_TYPE_UINT32,&type,
							 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);
	
	dbus_message_unref(query);
	
	if (NULL == reply) {
		if (dbus_error_is_set(&err)) {
			dbus_error_free(&err);
		}
		return SNMPD_CONNECTION_ERROR;
	}

	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
		retu = 1;
	else if (ret == ASD_SECURITY_NOT_EXIST) 		
		retu = -3;
	else if(ret == ASD_SECURITY_WLAN_SHOULD_BE_DISABLE) 		
		retu = -4;
	else
		retu = -5;

	dbus_message_unref(reply);
	return retu;
}


/*value must multiple of 15,range is 30-86400*/
int set_ap_max_detect_interval_cmd(dbus_parameter parameter, DBusConnection *connection,int id,char *value)
																				 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown format,please input number*/
																				 /*����-2��ʾthe number is not be multiple of 15������-3��ʾSecurity ID�Ƿ�*/
																				 /*����-4��ʾsecurity profile does not exist������-5��ʾthis security profile is used by some wlans,please disable them first*/
																				 /*����-6��ʾerror*/
{	
	if(NULL == connection)
			return 0;
		
	if(NULL == value)
		return 0;

	int ret = ASD_DBUS_SUCCESS;
	unsigned char security_id = 0;
	unsigned int interval = 0;
	DBusMessage *query, *reply; 
	DBusMessageIter  iter;
	DBusError err;
	int retu = 0;
	
	ret = parse_int_ID((char*)value, &interval);
	if(ret != ASD_DBUS_SUCCESS){
		return -1;
	}
	if( 0 != interval%15){
		return -2;

	}
	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];

	security_id = id;
	if(security_id >= WLAN_NUM || security_id == 0){
		syslog(LOG_DEBUG,"security id in set_ap_max_detect_interval_cmd is %d\n",security_id);
		return -3;
	}

	ReInitDbusPath_V2(parameter.local_id,parameter.instance_id,ASD_DBUS_BUSNAME,BUSNAME);
	ReInitDbusPath_V2(parameter.local_id,parameter.instance_id,ASD_DBUS_SECURITY_OBJPATH,OBJPATH);
	ReInitDbusPath_V2(parameter.local_id,parameter.instance_id,ASD_DBUS_SECURITY_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,ASD_DBUS_AP_DETECT_INTERVAL);

	dbus_error_init(&err);
	dbus_message_append_args(query,
							 DBUS_TYPE_BYTE,&security_id,
							 DBUS_TYPE_UINT32,&interval,
							 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (connection,query,-1, &err);
	
	dbus_message_unref(query);
	
	if (NULL == reply) {
		if (dbus_error_is_set(&err)) {
			dbus_error_free(&err);
		}
		return 0;
	}

	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == ASD_DBUS_SUCCESS)
		retu = 1;
	else if (ret == ASD_SECURITY_NOT_EXIST) 		
		retu = -4;
	else if(ret == ASD_SECURITY_WLAN_SHOULD_BE_DISABLE) 		
		retu = -5;
	else
		retu = -6;
	
	dbus_message_unref(reply);
	return retu;
}


#ifdef __cplusplus
}
#endif

