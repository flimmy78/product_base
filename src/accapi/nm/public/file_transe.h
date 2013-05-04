#ifndef _FILE_TRANSE_H
#define _FILE_TRANSE_H

typedef enum{
	LOAD_FLAG_BEGIN=0,
	DOWNLOAD,
	UPLOAD,
	LOAD_FLAG_END
}LOAD_FLAG;

typedef enum{
	TRANS_FLAG_FOREGROUND=1,		/*����������ǰִ̨��*/
	TRANS_FLAG_BACKGROUND
}TRANS_FLAG;


typedef enum{
	PROTOCAL_BEGIN=0,
	PROTOCAL_TFTP,
	PROTOCAL_FTP,				/*����Э���ϣ����������������Э����ʱ�����ǡ�*/
	PROTOCAL_SFTP,
	PROTOCAL_HTTP,
	PROTOCAL_HTTPS,
	PROTOCAL_END
}TRANS_PROTOCAL;


typedef enum{
	TRANS_STATUS_BEGIN=0,
	TRANSFERRING,
	COMPLETED,
	FAILED,
	TRANS_STATUS_END
}TRANS_STATUS;

typedef struct file_trans_t file_trans_t;
typedef int (*ON_TRANS_FINISH)( struct file_trans_t *me, void *param );

file_trans_t *file_trans_create();
int file_trans_destroy( file_trans_t *me );

int ft_set_loadflag( file_trans_t *me, LOAD_FLAG flag);
int ft_set_protocal( file_trans_t *me, TRANS_PROTOCAL portal);
int ft_set_ipaddr  ( file_trans_t *me, char *ipaddr );
int ft_set_port    ( file_trans_t *me, unsigned int port );
int ft_set_srvpath( file_trans_t *me, char *srvpath);
int ft_set_userpath(file_trans_t *me, char *userpath);
int ft_set_username( file_trans_t *me, char *username );
int ft_set_password( file_trans_t *me, char *password );
int ft_set_onfinish( file_trans_t *me, ON_TRANS_FINISH on_finish );

#define FT_OK						0
#define FT_ERR					(FT_OK-1)
#define FT_MALLOC_ERR		(FT_OK-2)
#define FT_POINTER_NULL	(FT_OK-3)

#define MAX_LEN 256

/*
*�⼸��get���岻�󣬿����Ȳ�ʵ�֡�
LOAD_FLAG 			ft_get_loadflag ( file_trans_t *me );
char 						*ft_set_filename( file_trans_t *me );
TRANS_PROTOCAL 	ft_set_protocal ( file_trans_t *me );
unsigned int 		ft_set_ipaddr   ( file_trans_t *me );
int 						ft_get_port     ( file_trans_t *me );
char 						*ft_get_username( file_trans_t *me );
char 						*ft_set_password( file_trans_t *me );
*/

TRANS_STATUS 		ft_get_status		( file_trans_t *me );
char 				*ft_get_failedreason( file_trans_t *me );



/*the pivotal function*/
/*
*	ִ���ļ����䶯����
*trans_flag = TRANS_FLAG_FOREGROUND: ���ļ����������ŷ���
*trans_flag = TRANS_FLAG_BACKGROUND: �����������أ��ļ��ں�̨���䡣
TRANS_FLAG_BACKGROUND���ģʽ�£��ڴ���Ĺ����п��Ե�������ĺ�������ȡһЩ�����״̬
int 	  ft_get_curent_trans_size( file_trans_t *me );->����ļ��Դ���Ĳ��ֵĴ�С��
TRANS_STATUS 		ft_get_status		( file_trans_t *me );->�õ���ǰ�Ĵ���״̬��ΪTRANS_STATUS��֮һ
**/
int		ft_do_transfers( file_trans_t *me);


/****************************************************
*	��ִ�еġ�trans_flag = TRANS_FLAG_BACKGROUND������LOAD_FLAGΪDOWNLOAD��ʱ��
*ͨ����������õ���ǰ�ļ��Ѿ������˶����ֽڡ�
*
*����LOAD_FLAGΪUPLOAD�������Ҫ�õ��Ѿ������˶����ֽ��Ǻ��Ƚ����ѡ����Կ���һ�¡�
*****************************************************/
int 	  ft_get_curent_trans_size( file_trans_t *me );




#endif
