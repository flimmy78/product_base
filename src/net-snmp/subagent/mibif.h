#ifndef _MIBIF_H
#define _MIBIF_H

#define MIBIF_DEBUG		0

#if MIBIF_DEBUG
#define MIBIF_PRINTF	printf("%s---%d:",__FILE__, __LINE__);printf
#else
#define MIBIF_PRINTF(...)
#endif
	

#define MAX_MIBIF_IFNAME_LEN		32
#define MAX_MIBIF_SUBIF_INFO_LEN	256	



typedef struct mibif_t {
	char ifname[MAX_MIBIF_IFNAME_LEN];
	int  gindex;
	int	 index;
	unsigned int ipaddr;
	unsigned int mask;
} mibif;

typedef struct mibif_lst_t{
	int mibif_num;
	int buf_mibif_num;
	mibif *lst;
} mibiflst;

typedef struct mibsubif_t {
	char ifname[MAX_MIBIF_IFNAME_LEN];
	int  gindex;
	int	 index;/*read only*/
	unsigned int ipaddr;
	unsigned int mask;
	int  subif_vlanid;/*for sub if*/
	char main_ifname[MAX_MIBIF_IFNAME_LEN];
	char info[MAX_MIBIF_SUBIF_INFO_LEN];/*for sub if*/
} mibsubif;

typedef struct mibsubif_lst_t{
	int mibif_num;
	int buf_mibif_num;
	mibsubif *lst;
} mibsubiflst;

/*0���õ���ǰ��������ӿڵ��б�*/
int get_all_mibif( mibiflst *iflst );/*ʹ����ɺ���Ҫ�ͷ�!!!!  ����ֵΪif����*/
int get_all_submibif( mibsubiflst *subiflst );/*ʹ����ɺ���Ҫ�ͷ�!!!!  ����ֵΪif����*/

/*1���õ���ǰ�ӿ��Ƿ�Ϊ���ؿ�*/
int if_main_mibif( const char *name );
/*ֻ����Щ�ӿ��²��ܴ����ӽӿڣ���ǰ������Ժ�ǿ��ֻ������7000��0-1~0-4,��ǰ5612�����ؿڣ����ǻ����ܴ����ӽӿ�,��ʵ�ʾ��������溯���н���ifname�Ĺ���*/


/*2���õ���ǰ�ӿ��Ƿ�Ϊ�ӽӿ�*/
int if_sub_mibif( const char *name );
/*ͬ�ϣ�Ҳ�Ǹ���ifname�����ģ���ǰֻ�����7000�������豸���ò���*/


/*3�������ӽӿڣ�����ӽӿڵ��������֣���Ҫ���ļ������档ʹ����򵥵��ı��ļ���*/
int create_sub_mibif( int vlanid, char *main_if, char *info );
	
	
/*4��ɾ���ӽӿڣ�*/
int delete_sub_mibif( int vlanid, char *main_if );
	
	
/*5���޸�����ӿڵ�ip��ַ*/
int set_mibif_ip( mibif *mif, unsigned int ipaddr );


/*6���޸�����ӿڵ�mask*/
int set_mibif_mask( mibif *mif, unsigned int mask );

#define SIOCGGINDEX		0x8939		/* get global index of the device */
#define SIOCSGINDEX 	0x893A		/* set global index of the device */
/*�޸�����ӿڵ�gindex*/
int set_mibif_gindex( mibif *mif, unsigned int gindex );

int mod_sub_mibif_mainif( int vlanid, char *old_main_if, char *new_main_if );
int mod_sub_mibif_gindex( int vlanid, char *main_if, int gindex );

#endif
