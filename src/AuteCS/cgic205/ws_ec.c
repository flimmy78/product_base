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
* ws_ec.c
*
*
* CREATOR:
* autelan.software.Network Dep. team
* qiaojie@autelan.com
* shaojw@autelan.com
* tangsq@autelan.com
* zhouym@autelan.com
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


#include "ws_ec.h"
#include "cgic.h"
#include <sys/wait.h>


typedef enum {
	LANG_EN,
	LANG_CH
}Enum_LANGUAGE;



FILE *fpp;                                /*����ȫ�ֱ�����fpp�ļ�ָ��*/
char ch;//,var[20];                         /*ch���ļ���ȡ�ַ�����ʱ������var���Ҫ���ҵı�����*/
int len;                                 /*���var�ĳ���*/



static void show_error( char *err_msg )
{
	if( NULL == err_msg )
	{
		err_msg = "Unknow Error!";
	}
	cgiHeaderContentType("text/html");
	fprintf(cgiOut,"<html><head>");
	fprintf(cgiOut,"<meta http-equiv=Content-Type content=text/html; charset=gb2312>");
	fprintf(cgiOut,"<title>Open Error</title>");
	fprintf(cgiOut,"<style type=text/css></style></head><body topmargin=130>"\
	"<form method=post action=/index.html>"\
	"<div align=center>"\
	"<SCRIPT  LANGUAGE=JavaScript>"\
	"alert(\"%s\")"\
	"</SCRIPT>"\
	"</div>"\
	"</form>"\
	"</body>"\
	"</html>", err_msg);	
	return;
}

char *ser_var(char *v)
{
  char flag[3],*r,*r1;                       /*flag[3]���lan��ֵ����������Ӣ�ģ�r��ŷ��ص�ָ��*/
  int fl;                                /*flָʾ����ʱ��������ĸ�����fl=0,��ʾ����Ӣ������fl=1��ʾ������������*/
  struct list *le,*lc;
  
  if((fpp=fopen("../htdocs/text/en-ch.txt","r"))==NULL)         /*��ֻ����ʽ����Դ�ļ�*/
  {
	show_error( ser_var("error_open") );
	return NULL;
  }
  
  fseek(fpp,4,0);                         /*���ļ�ָ���Ƶ����ļ���4���ֽڴ�����lan=֮��*/
  fgets(flag,3,fpp);                      /*��ȡ2λ�ַ�����flag[3]����ֵΪ"en"��"ch",flag[2]�д�Ŵ�������־"\0"*/
  ch=fgetc(fpp);
  while((ch==';')||(ch==' ')||(ch=='\n'))   /*�ƶ��ļ�ָ�룬ֱ����һ������*/
    ch=fgetc(fpp);
  fl=0;                                  /*��fl��ʼ��Ϊ0����һ�ν���ʱ�������Ӣ������*/
  if((le=(struct list *)malloc(sizeof(struct list)))==NULL)     /*��ʼ��Ӣ�������ͷ�ڵ�*/
  {
  	fclose(fpp);      
	show_error( ser_var("malloc_error") );
	return NULL;
  }
  le->next=NULL;                       
  if((lc=(struct list *)malloc(sizeof(struct list)))==NULL)     /*��ʼ�����������ͷ�ڵ�*/
  {
  	fclose(fpp);      
	show_error( "malloc_error" );
	return NULL;
  }
  lc->next=NULL;
  while(ch!=EOF)
  {
    if(!fl)                             /*fl==0,����������Ӣ������*/
    {
      if(load(le)==0)
      {
      	  fclose(fpp);      	
		  return NULL;
      }
      fl=!fl;                           /*flȡ������֤�������Խ��������Ӣ������*/
    }
    else if(fl)                         /*fl==1,������������������*/
         {
           if(load(lc)==0)
           {
           	fclose(fpp);      
			return NULL;
           }
           fl=!fl;                      /*flȡ������֤�������Խ��������Ӣ������*/
	}                              
    ch=fgetc(fpp);
    while((ch==' ')||(ch=='\n'))        /*Ѱ����һ��������ʼ��λ��*/
      ch=fgetc(fpp);
  }
  fclose(fpp);                           /*������ɣ��ر��ļ�*/
  len=strlen(v);
  if(strcmp(flag,"en")==0)              /*flag=="en"����Ӣ�������в��ұ���v*/
    r=search(le,v);
  else if(strcmp(flag,"ch")==0)         /*flag=="ch"�������������в��ұ���v*/
    r=search(lc,v);  
  r1 = (char*)malloc(strlen(r)+1);
  memset(r1, 0, strlen(r)+1);
  memcpy(r1, r, strlen(r));
  release(le);
  release(lc);
  return(r1);
}        

int equal(char *des,char *src)        /*�Ƚ��ַ���src���ַ���des��ǰsizeof(src)���ַ��Ƿ���ȣ���ȷ���1������ȷ���0*/
{
  int i,f,l;
  f=1;
  i=0;
  l=strlen(src);
  while(f&&(i<l))
    if(*(des+i)==*(src+i))
      i++;
    else
      f=0;
  if(f&&(*(des+i)=='='))                  /*��֤des��"="ǰ��������src���*/
    return 1;
  else
    return 0;
}

int load(struct list *l)               /*�ָ��ļ�fp�еı��������δ�������l*/
{
  struct list *p;
  int i;
  int n_ex=1;

  if((p=(struct list *)malloc(sizeof(struct list)+n_ex*LN))==NULL)
  {
	show_error( "Malloc error!" );
	return 0;
  }
  i=0;
  memset(p, 0, sizeof(struct list)+n_ex*LN);
  
  while(ch!=';')                        /*";"��������ֵ�Ľ�����־*/
  {
    p->val[i]=ch;
    i++;
	if( i >= n_ex*LN-1 )//������ȳ���LN,���·��ɿռ䡣
	{
	
		n_ex++;
		if((p=(struct list *)realloc(p,sizeof(struct list)+n_ex*LN))==NULL)
		{
			show_error( "realloc error!" );
			return 0;
		}
	}
    ch=fgetc(fpp);
  }

  p->val[i]='\0';                     /*�����ַ���������־'\0'*/
  
  p->next=l->next;                      /*���µĽڵ��������*/
  l->next=p;
  return 1;
}

char *search(struct list *l,char *v)    /*������l�в��ұ���v������ָ�������ֵ��ָ�룬û�ҵ�����NULL*/
{
  struct list *p=NULL;
  char *rs=NULL;
  int i,find;                           /*find��ʾ�Ƿ��ҵ�������find=0��ʾ�ҵ���find=1��ʾû�ҵ�*/

  if( NULL != l )
  {
  	p=l->next;
  }
  find=1;
  
  len=strlen(v);
  while((p!=NULL)&&(find))              /*���û���ҵ�������û�н���*/
  {
    if(equal(p->val,v))                 /*�ҵ��������ڵĽڵ�*/
    {
      i=len+1;                          /*����"="*/
	  rs=p->val;
      rs+=i;                           /*rsָ�����ֵ�Ŀ�ʼ��ַ*/
      find=0;                          /*�ҵ��������޸ı�־*/
    }
    else                               /*û���ҵ����������*/
      p=p->next;
  }
  if(find)                             /*find=1��ʾû�ҵ�����*/
    return STR_NOT_FOND;//û���ҵ��ַ�����ʱ�����������
  else
	return(rs);
}

void release(struct list *l)
{
  struct list *p1,*p2;
  if(NULL != l)
  {
	  p1=l; 							   /*p1ָ��ͷ�ڵ�*/
	  p2=p1->next;						   /*p2ָ��ͷ�ڵ����һ���ڵ�*/  
	  free(p1); 						   /*�ͷ�ͷ�ڵ�*/
	  p1=p2;							   /*p1���ƣ�ָ����һ�����ͷŵĽڵ�*/  
	  while(!p1)
	  {
		p2=p1->next;					   /*p2ָ����ͷŽڵ����һ�ڵ�*/
		free(p1);					  
		p1=p2;
	  }
  }
}

char *getFPath(char * v)
{
	char *path;
	char *path1;
	struct list *lc;
	if((fpp=fopen("/usr/local/apache2/htdocs/text/web_info.txt","r"))==NULL)		 /*��ֻ����ʽ����Դ�ļ�*/
	{
		show_error( "Cannot open file!" );
	  	return NULL;
	}
    fseek(fpp,0,0);
	ch=fgetc(fpp);
	while((ch==';')||(ch==' ')||(ch=='\n'))   /*�ƶ��ļ�ָ�룬ֱ����һ������*/
	  ch=fgetc(fpp);			 
	if((lc=(struct list *)malloc(sizeof(struct list)))==NULL)	  /*��ʼ�������ͷ�ڵ�*/
	{
		fclose(fpp);      
		show_error( "Malloc error!" );
		return NULL;
	}
	lc->next=NULL;
	while(ch!=EOF)
	{
	  	if(load(lc)==0)
	  	{
	  		fclose(fpp);      
			return NULL;
	  	}
	  	ch=fgetc(fpp);
	  	while((ch==' ')||(ch=='\n'))		  /*Ѱ����һ��������ʼ��λ��*/
			ch=fgetc(fpp);
	}
	fclose(fpp);						   /*������ɣ��ر��ļ�*/
	len=strlen(v);
	path=search(lc,v);          /*�������в��ұ���v*/
	path1 = (char*)malloc(strlen(path)+File_Len+5);    /*File_Len���ϴ�ͼƬ������󳤶�*/
	memset(path1, 0, strlen(path)+File_Len+5);
	memcpy(path1, path, strlen(path));
	release(lc);
	return(path1);

}

char * readproductID()
{
	char * syscommand=(char *)malloc(100);
	 memset(syscommand,0,100);
	 char temp[10];
	FILE * ft;
	
	strcat(syscommand,"cat /proc/product_id  >/var/run/apache2/product_id.txt");
	int status = system(syscommand);
	int ret = WEXITSTATUS(status);
						 
	if(0==ret)
		{}
		//fprintf(stderr,"showRoute=%s",showRoute);
	else
	{
			return "Switch7000";
	}
	
	if((ft=fopen("/var/run/apache2/product_id.txt","r"))==NULL)
	{
			show_error( "Cannot open file!" );
			return NULL;
	}

	fgets(temp,10,ft);
	fclose(ft);
/*
  * Full product type id definition, communicated between control processes and management processes
  * currently supported product id as follows:
  *		PRODUCT_ID_AX7K 	- 0xC5A000	(chassis,5 slots, marvell, cheetah2 275)
  *		PRODUCT_ID_AU4K 	- 0xB1A100	(box, 1 slots, marvell, cheetah2 265)
  *		PRODUCT_ID_AU3K 	- 0xB1A200	(box, 1 slots, marvell, cheetah2 255)
  *		PRODUCT_ID_AU3K_BCM - 0xB1B000 	(box, 1 slots, broadcom, raven 56024B0)
  *
*/
	if(0==strcmp(temp,"12951553"))  //MODULE_ID_AX7_CRSMU = PRODUCT_ID_AX7K + 1, 
	{
		return "Switch7000";
	}
	else if(0==strcmp(temp,"11641089"))   //MODULE_ID_AU4_4626 = PRODUCT_ID_AU4K + 1, 
	{
		return "Switch5000";
	}
	else if(0==strcmp(temp,"11641601"))    //MODULE_ID_AU4_5612 = PRODUCT_ID_AU4K + 1, 
	{
		return "Switch5612";
	}
	else if(0==strcmp(temp,"11644930"))   //MODULE_ID_AU4_3052 = PRODUCT_ID_AU4K + 1, 
	{
		return "Switch3052";
	}
	else if(0==strcmp(temp,"11641857"))   //MODULE_ID_AU4_3052 = PRODUCT_ID_AU4K + 1, 
	{
		return "Switch5612i";
	}
	else return "Switch5000";
}



//----------------------------------------------------------------------
//
//  ������:int getfpath(const char *v, char *fpath);
//    ����:����û���web_info.txt����ӵ��ļ�·��
//�������:const char *v�ļ�·����־��char *fpath����û���Ҫ��·��
//�������:����0�ɹ�������-1ʧ�ܣ�
//    ����:Ǯ��ΰ
//    
//----------------------------------------------------------------------
int getfpath(const char *v, char *fpath )  
{
    FILE *fp;
    char *buff=(char *)malloc(1024);
    const char *flag="=";
    char buffer[1024];
    int i;
    int ret=-1;
    char ch,*path,*file;
    if((fp=fopen("../htdocs/text/web_info.txt","r"))==NULL)
    {
        fprintf(stderr,"cann't open web_info.txt!");
		ret=-1;
    }
    else
    {
        i=0;
        memset(buff,0,1024);
        ch=fgetc(fp);
                while(ch!=EOF)   /*�ƶ��ļ�ָ�룬ֱ����һ������*/
                {
                	
                        if(ch==';')
                        {
                                buffer[i] = '\0';
                                strcpy(buff,buffer);
                                if((path=strstr(buff,v))!=NULL)
                                {
                                        if((file=strstr(path,flag))!=NULL)
                                        {
                                                file=file+1;
                                                //printf("%s",file);
                                                strcpy(fpath,file);
                                                ret=0;
                                        }
                                        break;
                                }
                                ch=fgetc(fp);
								
                                while((ch==' ')||(ch=='\n'))
									ch=fgetc(fp);
								
								i=0;                          /*������ԭ���ڴ�����*/
								memset(buffer,0,1024);
								memset(buff,0,1024);
								
                        }
                        else
                        {
                                buffer[i] = ch;
                                i++;
                                ch=fgetc(fp);
                        }

                }
				fclose(fp);
    }

    free(buff);
    return ret;
}


/***************************************************************
*USEAGE:to get the language of current user's browser set.
*Param: 
*Return:	Enum_LANGUAGE is an enum which defined in ws_ec.h
*Auther:shao jun wu
*Date:2008-12-2 11:09:51
*Modify:(include modifyer,for what resease,date)
****************************************************************/
static Enum_LANGUAGE get_language_type()
{
	char *en_pos=NULL;
	char *zh_pos=NULL;
	char *browser_language = getenv("HTTP_ACCEPT_LANGUAGE");//��õ�ǰ��������õ�������Ϣ
	Enum_LANGUAGE e_ret=LANG_CH;

	if( NULL != browser_language )
	{
		en_pos = strstr( browser_language, "en" );//en��language�е�λ��
		zh_pos = strstr( browser_language, "zh-cn" );//zh��language�е�λ��
		if( NULL == zh_pos )
		{
			zh_pos = strstr( browser_language, "zh-CN" );
		}
	}
	//�ĸ���ǰ�棬�����������ԡ������û�У�Ĭ��ʹ��Ӣ�ġ�
	if( zh_pos != NULL && (NULL == en_pos || zh_pos < en_pos) ) 
	{
		e_ret = LANG_CH;
	}
	else
	{
		e_ret = LANG_EN;
	}
	return e_ret;		
}


/***************************************************************
*USEAGE:static func which show language file load error!
*Param: 
*Return:	file_name->the file load
		err -> error no , defined below
*Auther:shao jun wu
*Date:2008-12-2 11:09:51
*Modify:(include modifyer,for what resease,date)
****************************************************************/

#define FILE_PATH_ERR	0
#define FILE_OPEN_ERR	1
#define MALLOC_ERR		2
static void language_err_proc( char *file_name, int err )
{
	char *err_msg = "Unknown error!";
	
	switch(err)
	{
		case FILE_PATH_ERR:
			err_msg = "File Path is Null!";
			break;
		case FILE_OPEN_ERR:
			err_msg = "File open error!";
			break;
		case MALLOC_ERR:
			err_msg = "Malloc error!";
			break;
		default:
			break;
	}

   show_error( err_msg );

   return;
}


/***************************************************************
*USEAGE:�õ���ǰ���Ե�����
*Param: 	file_name->file name which to load
*Return:	list -> root node of language list
*Auther:shao jun wu
*Date:2008-12-2 11:29:51
*Modify:(include modifyer,for what resease,date)
****************************************************************/

struct list *get_chain_head(char *file_name)          /*file_name��ʾ��Դ�ļ������֣����������ͷ*/
{
//	FILE *fp;
//	char flag[3];                             /*flag[3]���lan��ֵ����������Ӣ��*/
	int fl;                                   /*flָʾ����ʱ��������ĸ�����fl=0,��ʾ����Ӣ������fl=1��ʾ������������*/
	struct list *le,*lc;

	if( NULL == file_name || strlen(file_name) == 0 )
	{
		language_err_proc( file_name, FILE_PATH_ERR );
		return NULL;
	}

	if((fpp=fopen(file_name,"r"))==NULL)    /*��ֻ����ʽ����Դ�ļ�*/
	{
		language_err_proc( file_name, FILE_OPEN_ERR );
		return NULL;
	} 
	fl=0;                                  /*��fl��ʼ��Ϊ0����һ�ν���ʱ�������Ӣ������*/
	if((le=(struct list *)malloc(sizeof(struct list)))==NULL)     /*��ʼ��Ӣ�������ͷ�ڵ�*/
	{
		fclose(fpp); 
	  	language_err_proc( file_name, MALLOC_ERR );
   		return NULL;
  	}
	le->next=NULL;                       
	if((lc=(struct list *)malloc(sizeof(struct list)))==NULL)     /*��ʼ�����������ͷ�ڵ�*/
	{
		fclose(fpp); 
	  	language_err_proc( file_name, MALLOC_ERR );
		return NULL;
  	}
	lc->next=NULL;
	ch=fgetc(fpp);
	while(ch!=EOF)
	{
    	if(!fl)                             /*fl==0,����������Ӣ������*/
	    {
    		if(load(le)==0)
    		{
    			fclose(fpp); 
		  		return NULL;
    		}
      		fl=!fl;                           /*flȡ������֤�������Խ��������Ӣ������*/
    	}
    	else if(fl)                         /*fl==1,������������������*/
    	{
        	if(load(lc)==0)
        	{
        		fclose(fpp); 
				return NULL;
        	}
           	fl=!fl;                      /*flȡ������֤�������Խ��������Ӣ������*/
		}                              
	    ch=fgetc(fpp);
    	while((ch==' ')||(ch=='\n') ||(0x0d==ch)||(0x0a==ch))        /*Ѱ����һ��������ʼ��λ��*/
	    {
	    	ch=fgetc(fpp);
    	}
  	}
  	fclose(fpp);                           /*������ɣ��ر��ļ�*/
	fpp = NULL;
  	if(LANG_EN == get_language_type() )              /*flag=="en"*/
  	{
    	release(lc);                        /*�ͷ���������ͷ*/	
		return le;							 /*����Ӣ������ͷ*/ 
  	}
  	else                                  /*flag=="ch"*/
  	{ 
   		release(le);                        /*�ͷ�Ӣ������ͷ*/	
   		return lc;                           /*������������ͷ*/
  	}
	
  	return NULL;
}       

//url�������ַ�
char *replace_url(char *strbuf, char *sstr, char *dstr)
{       char *p,*p1;
        int len;

        if ((strbuf == NULL)||(sstr == NULL)||(dstr == NULL))
                return NULL;

        p = strstr(strbuf, sstr);       //�����ַ�����һ�γ��ֵĵ�ַ,���򷵻�NULL
        if (p == NULL)  /*not found*/
                return NULL;

        len = strlen(strbuf) + strlen(dstr) - strlen(sstr);
        p1 = malloc(len);
        bzero(p1, len);
        strncpy(p1, strbuf, p-strbuf);
        strcat(p1, dstr);
        p += strlen(sstr);
        strcat(p1, p);
        return p1;
} 
void cgiGetenv_func(char **s, char *var)
{
	*s = getenv(var);
	if (!(*s)) 
	{
		*s = "";
	}
}


/*********************************************************

	һЩ���ܱ����ģ����õ��Ĺ�������

***********************************************************/
/*******************************************************
		
********************************************************/

#ifdef __cplusplus
}
#endif



