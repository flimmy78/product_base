#ifndef _MAC_CORP_H
#define _MAC_CORP_H


int init_mac_corp_data();

/*�����mac��ʽ�ǣ�16���ƴ�д����'-'�ָ�*/
int get_corp_by_mac( char *mac, char *corp, int buff_len );

int destroy_mac_corp_data();


/*���ֻ��Ҫ�õ�һ��mac��corp������������꣬����������õ������������ĺ���Ч�ʸ��ߣ�*/
#define get_corp_by_mac_ext( mac, corp, len ) \
		if( init_mac_corp_data() == 0 ) {\
			get_corp_by_mac(mac,corp,len);\
			destroy_mac_corp_data();\
		}


#endif
