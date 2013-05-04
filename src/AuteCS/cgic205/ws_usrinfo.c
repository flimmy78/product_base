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
* ws_usrinfo.c
*
*
* CREATOR:
* autelan.software.Network Dep. team
* zhouym@autelan.com
*
* DESCRIPTION:
* function for web
*
*
***************************************************************************/
#include <stdio.h>
/*#include "cgic.h"*/
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>
#include "ws_usrinfo.h"
#include <libxml/xpathInternals.h>


const char* flag = "**";

#define CONFIG_PRE_MARK "CONFIG FILE LOCATION:"   /*���������ļ�·���ı�־*/
#define MY_ENCODING "utf-8"


///////��̬����������/////////
static int check_ip_part(char * src);
static int CHECK_IP_DOT(int a);


/*
        ���ܣ����û���������м��ܲ�ǩȨ
        ����: ���ܳɹ����ؼ��ܺ���ַ���
              "0":�Ƿ��û�
              "-2":��Դ�ļ�log_inf.txt δ��
              "-3":������ʱ�ռ�ʧ��
*/

char* encryption(char *user, char *pass)
{
    char buf[BUF_LEN];                                  /* ��ż��ܺ���ַ��� */
    char str[BUF_LEN];                                    /* ���ؼ��ܺ�Ĵ�*/
    char* rtn = (char *)malloc(BUF_LEN);
    int i,j;
    strcpy(str,user);
    strcat(str, flag);                                  /*�û���������ķָ���*/
    strcat(str, pass);
    
    /*�򵥼���*/
    for (i = 0; i < strlen(str); ++i)
    {
        for(j = 1; j < 4; j++)
        {
            buf[i] = (char)((int)(str[i]) + j);
        }
        
    }
    buf[i] = '\0';
    strcpy(rtn, buf);
    
    return     rtn;
}



/*
        ���ܣ������ܺ�Ĵ����ܲ�����ǩȨ
        ����: ���ܳɹ����ǺϷ��û����򷵻��û���
              �Ƿ��û��򷵻�NULL
*/
/*char* dcryption(char *str)
{
     char  buf[BUF_LEN];               */               /* ��Ž��ܺ���ַ��� */
     /*char* rtn = (char *)malloc(BUF_LEN);
	 char *command = (char *)malloc(PATH_LENG); 
     char* usr;
     char* pass;
     int i,j,ret,status,ret1;

     memset(rtn,0,BUF_LEN);
     memset(buf,0,BUF_LEN);
     if(strlen(str)<= 0)
     {
             return 0;    
     }*/
         /* ���� */
        /*for (i = 0; i < strlen(str); ++i)
        {
            for(j = 1; j < 4; j++)
    		{
                buf[i] = (char)((int)(str[i]) - j);
    		}
          
        }
        buf[i] = '\0';
        
        usr = strtok(buf, flag);  */          /*�����ַ���*/
       /* pass = strtok(NULL, flag);
        strcpy(rtn, usr);
        ret = checkuser_exist(rtn);
		if(ret == -1)
			return NULL;
		else
		{
		   memset(command,0,PATH_LENG);
		   strcat(command,"checkpwd.sh");   
		   strcat(command," ");
		   strcat(command,usr);
		   strcat(command," ");
		   strcat(command,pass);
		   strcat(command," ");
		   strcat(command,"11");
		   
           status = system(command);     */   /*��½��֤*/
		   /*free(command);
           ret1 = WEXITSTATUS(status);
		
           if(ret1 == 1)
		     return rtn;
		   else
		   	return NULL;
		}
		
}*/

/***********************************************/
/*
    ���ܣ��޸�session.xml�е�������ʱ��
    ���룺char *session_id:��¼��session ID
    ���أ�
                ��NULL:�޸ĳɹ������Խ���ϵͳ��
                NULL:�û��Ƿ���ʱ�������Խ���ϵͳ��
*/
/***********************************************/
char *dcryption_noreflesh(char *session_id)
{
	char *cookiestr = getenv("HTTP_COOKIE");
	char cstr[128] = {0};
	int timeout = 0;
	snprintf(cstr,sizeof(cstr),"user&%s",session_id);
	if (NULL == cookiestr)
	{
		return NULL;
	}
	else
	{
		if (0 == strcmp(cookiestr,cstr))
		{
			timeout = get_timeout_threshold();
			char ser_result[N] = {0};
			static char user_name[N];
			char *endptr = NULL;
			int isExist = 0,difftime = 0,last_time = 0;
			time_t now;
			unsigned int current_time = 0;
			int flag = 0;  /*1:��ʱ�û���2:δ��ʱ�û�*/
			isExist=Search_user_infor_byID(session_id,3,&ser_result);
			if(isExist==1) /*�û���Ϣ�Ѵ���*/
			{
				current_time = (unsigned int)time(&now);
				last_time= strtoul(ser_result,&endptr,10);	  /*charת��int��10����ʮ����*/				
				difftime=current_time-last_time;
				if(difftime>timeout)    /*���ʳ�ʱ*/
				{
					flag=1;
				}
				else
				{
					flag=2;
				}
			}
			else   /*�û��Ƿ�*/
			{
				return NULL;
			}

			if(flag==1) /*��ʱ�û�������ɾ��������Ϣ*/
			{
				del_user_infor(session_id);
				return NULL;
			}
			else   /*�޸�������ʱ��*/
			{
				//current_time=time((time_t*)NULL);
				//modify_Last_AccTime(session_id,current_time);
				memset(user_name,0,N);
				Search_user_infor_byID(session_id,1,user_name);
				return user_name;
			}

		}
		else
		{
			return NULL;
		}
	}
}
char* dcryption(char *session_id)
{
	char *cookiestr = getenv("HTTP_COOKIE");
	char cstr[128] = {0};
	int timeout = 0;
	snprintf(cstr,sizeof(cstr),"user&%s",session_id);
	if (NULL == cookiestr)
	{	
		
		return NULL;
	}
	else
	{
		
		if (0 == strcmp(cookiestr,cstr))
		{
		
			timeout = get_timeout_threshold();
			char ser_result[N] = {0};
			static char user_name[N];
			char *endptr = NULL;
			int isExist = 0, difftime = 0,last_time = 0;
			time_t now;
			unsigned int current_time = 0;
			int flag = 0;  /*1:��ʱ�û���2:δ��ʱ�û�*/
			current_time = (unsigned int)time(&now);
			isExist=Search_user_infor_byID(session_id,3,&ser_result);
			fprintf(stderr,"session_id======================%s\n",session_id);
			if(isExist==1) /*�û���Ϣ�Ѵ���*/
			{	
				fprintf(stderr,"timeout===%d\n",timeout);
				last_time= strtoul(ser_result,&endptr,10);	  /*charת��int��10����ʮ����*/	
				fprintf(stderr,"ser_result==-===%s\n",ser_result);
				fprintf(stderr,"last_time======%d\n",last_time);
				fprintf(stderr,"current_time======%u\n",current_time);
				difftime=current_time-last_time;
				fprintf(stderr,"difftime======%d\n",difftime);
				if(difftime>timeout)    /*���ʳ�ʱ*/
				{
				//	fprintf(stderr,"flag!!!!!!!!!!!!\n");
					flag=1;
				}
				else
				{
					//	fprintf(stderr,"hhdddhh!!!!!!!!!!!!\n");
					flag=2;
				}
			}
			else   /*�û��Ƿ�*/
			{
					//fprintf(stderr,"hhhh!!!!!!!!!!!!\n");
				return NULL;
			}

			if(flag==1) /*��ʱ�û�������ɾ��������Ϣ*/
			{
				//	fprintf(stderr,"uuuu!!!!!!!!!!!!\n");
				del_user_infor(session_id);
				return NULL;
			}
			else   /*�޸�������ʱ��*/
			{
				//	fprintf(stderr,"iiii!!!!!!!!!!!!\n");
				//current_time=time((time_t*)NULL);
				modify_Last_AccTime(session_id,current_time);
				memset(user_name,0,N);
				Search_user_infor_byID(session_id,1,user_name);
				return user_name;
			}

		}
		else
		{
				//fprintf(stderr,"NULL!!!!!!!!!!!!\n");
			return NULL;
		}
	}
}


int update_usrtime(char *session_id)
{
	char *ser_result=(char *)malloc(N);
	int flag=-1;
	int isExist;
	time_t now;
	unsigned int current_time = 0;
	memset(ser_result,0,N);
	isExist=Search_user_infor_byID(session_id,3,ser_result);
	if(isExist==1) 
	{
		current_time = (unsigned int)time(&now);
		flag=current_time;
		modify_Last_AccTime(session_id,current_time);
		//flag=0;
		//return 0;
	}
	else  
	{
	   // flag=1;
		//return -1;
	}
	return flag;
}
/***********************************************/
/*
    ���ܣ���ʾ���û���Ϣ
    ���룺char *groupname: ����
    ���أ�
              0���ɹ�
            -1��ʧ��
*/
/***********************************************/


/*int get_user(char* groupname)    
{
    struct group *grentry = NULL;
    char *ptr=NULL;
    int i;
    int cl = 1;

    if(!groupname)
        return -1;
    if(strcmp(groupname,ADMINGROUP)&&strcmp(groupname,VIEWGROUP))
        return -1;
    grentry = getgrnam(groupname);
    if (grentry)
    {
        for(i=0;(ptr=grentry->gr_mem[i])!=NULL;i++)
    	{        		  	
          fprintf(cgiOut,"<tr height=25 bgcolor=%s align=left style=\"padding-left:20px\" font-size:14px>");
           
          if(strcmp(groupname,ADMINGROUP)== 0)
            {
            	fprintf(cgiOut,"<td>%s</td>",setclour(cl) ,ptr);
                fprintf(cgiOut,"<td>administrator</td>");
                fprintf(cgiOut,"<td>&nbsp;</td>");
            }
          else
          	{
          		fprintf(cgiOut,"<td>%s</td>",setclour(cl) ,ptr);
                fprintf(cgiOut,"<td>user</td>");
                fprintf(cgiOut,"<td><a href=wp_usrlis.cgi?UN=%s&DELETE=%s target=mainFrame onclick=\"return confirm('%s')\" style=text-decoration:underline>%s</td>",encry,"delete",mode,authString,recversion,sendversion,horizon,intfName[i],address,search(lcontrol,"confirm_delete"),search(lcontrol,"delete"));
          	}
          fprintf(cgiOut,"</tr>");
          cl=!cl;			 
    	}		
    }
	
    return 0;
}*/

/***********************************************/
/*
    ���ܣ�����ɾ���û���Ϣ
    ���룺char *groupname: ����
                    char* current_user:��ǰ�û���
    ���أ�
              0���ɹ�
            -1��ʧ��
*/
/***********************************************/


int set_deluser(char* groupname, char* current_user, st_deluser *ulist_head)
{
    struct group *grentry = NULL;
    char *ptr=NULL;
    int i;
   // int cl = 1;
	st_deluser *user_new = NULL;
	st_deluser *user_tail = ulist_head;

	if(NULL==ulist_head)
		return -1;

	if(!groupname)
        return -1;
    if(strcmp(groupname,ADMINGROUP)&&strcmp(groupname,VIEWGROUP))
        return -1;
    grentry = getgrnam(groupname);
    if (grentry)
    {

        for(i=0;(ptr=grentry->gr_mem[i])!=NULL;i++)
    	{
          if(strcmp(current_user,ptr)!=0)  /*��ʾ����ǰ�û����û��б�*/
          	{
              //fprintf(cgiOut,"<tr height=25 bgcolor=%s align=left>",setclour(cl));
              //fprintf(cgiOut,"<td><input type=checkbox name=check value=%s></td>",ptr);
              //fprintf(cgiOut,"<td align=left style=font-size:14px>%s</td>",ptr);

			  user_new = (st_deluser *)malloc(sizeof(st_deluser));
			  if(NULL==user_new)
			  	return -1;
			  strcpy(user_new->name, ptr);	
			  user_new->next = NULL;
			  
			  user_tail->next = user_new;/*�ӵ���β*/
			  user_tail = user_new;
			  
              if(strcmp(groupname,ADMINGROUP)== 0)
              	{
                   // fprintf(cgiOut,"<td>administrator</td>");
                   strcpy(user_tail->group, "administrator");
              	}
              else
        	  	{
                    //fprintf(cgiOut,"<td>user</td>");
					strcpy(user_tail->group, "user");
        	  	}

              //fprintf(cgiOut,"</tr>");
             // cl=!cl;		
          	}

    	}
    		
    }
    return 0;

}
int free_deluser(st_deluser *ulist_head)
{
	st_deluser *user_this = ulist_head->next;

	if (NULL == ulist_head)
		return -1;
	
	while (NULL != user_this){/*�ͷ�������*/
		ulist_head->next = user_this->next;
		free(user_this);
		user_this = ulist_head->next;
	}
	
	return 0;
}

char *setclour(int i)         /*������ʾ��ɫ*/
{
  if(i==1)
    return("#f9fafe");
  else
    return("#ffffff");
}

/***********************************************/
/*
    ���ܣ�����û��Ƿ����
    ���룺char *username: �û���
    ���أ�
              0���ɹ�
            -1��ʧ��
*/
/***********************************************/

int checkuser_exist(char* username)
{
    struct group *grentry = NULL;
    char *ptr=NULL;
    int i;

    grentry = getgrnam(ADMINGROUP);
    if (grentry)
    {

        for(i=0;(ptr=grentry->gr_mem[i])!=NULL;i++)
          {
            if(strcmp(username,ptr) == 0)
              {
                    return 0;
              }
          }	
    }

    struct group *grentry1 = NULL;
    grentry1 = getgrnam(VIEWGROUP);
    if (grentry1)
    {   
        for(i=0;(ptr=grentry1->gr_mem[i])!=NULL;i++)
          {
            if(strcmp(username,ptr) == 0)
              {
                    return 0;
              }
          }
    		
    }	
    
    return -1;  
}

/***********************************************/
/*
    ���ܣ�����û������ĸ���
    ���룺char *username: �û���
    ���أ�
              0������Ա��
              1���û���
            -1���Ƿ��û�
*/
/***********************************************/


int checkuser_group(char* username)
{

    char *ptr = NULL;
    int i;
	
    struct group *grentry = NULL;        /*��ADMINGROUP���м��*/
    grentry = getgrnam(ADMINGROUP);
    if (grentry)
    {

        for(i=0;(ptr=grentry->gr_mem[i])!=NULL;i++)
          {
            if(strcmp(username,ptr) == 0)
              {
                    return 0;
              }
          }
 		
    }

	struct group *grentrys = NULL;      /*��VIEWGROUP���м��*/
	grentrys = getgrnam(VIEWGROUP);
    if (grentrys)
    {   
        for(i=0;(ptr=grentrys->gr_mem[i])!=NULL;i++)
          {
            if(strcmp(username,ptr) == 0)
              {
                    return 1;
              }
          }
    		
    }	
    
    return -1; 

}
/***********************************************/
/*
    ���ܣ����������ļ���·��
    ���룺integrate_default:������������ļ���·���� config_file:Quagga.conf������·��  
    ���أ�
              0���ɹ�
            -1��ʧ��
*/
/***********************************************/

int get_integrate_config_file(char *integrate_default, char *config_file)
{
        FILE* confp=NULL;
        char* buf = NULL;
        char* ptr = NULL;
        char tmp[128];
        int find = 0;


        memset(tmp,0,128);
        confp = fopen(config_file,"r");
        if (!confp )
          return -1;

        buf = (char*)malloc(128);
        if(!buf)
                return -1;
        while((ptr = fgets(tmp,128,confp))!=NULL)
        {
                if(strstr(tmp,CONFIG_PRE_MARK))
                {
                        memcpy(buf,tmp+strlen(CONFIG_PRE_MARK),strlen(tmp)-strlen(CONFIG_PRE_MARK)-1);
                        find =1;
                        break;
                }
        }
        fclose(confp);

        if(find == 0)
        {
                free(buf);
                buf = NULL;
        }
        strcpy(integrate_default,buf);
        return 0;
}


int checkpassword(char *password)
{
	int len,i;
	len = strlen(password);
	for(i=0;i<len;i++)
	{
			if((isalpha(password[i])==0)&&(isdigit(password[i])==0)&&(password[i]!='_'))
			{
					return -1;
			}
	}
	return 0;

}


int checkPoint(char *ptr)
{
	int ret = 0;
	while(*ptr != '\0')
		{
			if(((*ptr) < '0')||((*ptr) > '9'))
				{
					ret = 1;
					break;
				}
			*ptr++;
		}
	return ret;
}

int str2ulong(char *str,unsigned int *Value,int min_num,int max_num)
{
	char *endptr = NULL;
	char c;
	int ret = 0;
	if (NULL == str) return -1;

	ret = checkPoint(str);
	if(ret == 1){
		return -2;  //��������ϱ���
	}

	c = str[0];
		
	*Value= strtoul(str,&endptr,10);
	if('\0' != endptr[0]){
		return -1;
	}
	if(*Value<min_num  || *Value >max_num)
		return -3;   //��ֵ��Χ����
	return 0;	
}



void ParseDoc(xmlDocPtr doc)
{  
  xmlNodePtr cur; 				 /*����ڵ�ָ��*/
  xmlChar *key;
  cur=xmlDocGetRootElement(doc);	  /*ȷ���ĵ���Ԫ��*/
  if(cur==NULL)
  {
    fprintf(stderr,"empty document.");
    xmlFreeDoc(doc);
	return;
  }
  if(xmlStrcmp(cur->name,(const xmlChar *)"root"))
  {
    fprintf(stderr,"document of the wrong type,root node!=root");
    xmlFreeDoc(doc);
    return;
  }
  cur=cur->xmlChildrenNode;
  while(cur!=NULL)
  {
    if((!xmlStrcmp(cur->name,(const xmlChar *)"keyword")))
    {
      key=xmlNodeListGetString(doc,cur->xmlChildrenNode,1);
	  fprintf(stderr,"keyword:%s",key);
	  xmlFree(key);
    }
	cur=cur->next;
  }
}


xmlXPathObjectPtr get_nodeset(xmlDocPtr doc, const xmlChar *xpath) 
{
  xmlXPathContextPtr context;
  xmlXPathObjectPtr result;

  context = xmlXPathNewContext(doc);
  if (context == NULL) 
  {
    fprintf(stderr,"context is NULL.");
    return NULL;
  }

  result = xmlXPathEvalExpression(xpath, context);
  xmlXPathFreeContext(context);
  if (result == NULL) 
  {
    fprintf(stderr,"xmlXPathEvalExpression return NULL.");
    return NULL;
  }

  if (xmlXPathNodeSetIsEmpty(result->nodesetval)) 
  {
    xmlXPathFreeObject(result);
    fprintf(stderr,"nodeset is empty.");
    return NULL;
  }
  return result;
}



/***********************************************/
/*
    ���ܣ������û���Ϣ�ļ�
    ���룺��
    ���أ�
              1���ɹ�
              0��ʧ��
*/
/***********************************************/

int new_user_profile()
{
	FILE *fp;
	if((fp=fopen(USER_LOG_IN,"w"))==NULL)		
		return 0;
	fputs("<?xml version='1.0' encoding='utf-8'?><root><userCount>0</userCount></root>",fp);
	fclose(fp);
	return 1;
}



void Add_session_Node(xmlDocPtr doc,SESSION_PROFILE session)
{
  xmlNodePtr cur; 				 /*����ڵ�ָ��*/
  xmlNodePtr p,q;
  xmlChar *xpath=BAD_CAST "/";
  int i;
  char *tempor=(char *)malloc(N);

  xmlXPathObjectPtr app_result=get_nodeset(doc,xpath);
  if(app_result==NULL)
  {
    fprintf(stderr,"app_result is NULL.");
	return;
  }

  if(app_result)
  {
    xmlNodeSetPtr nodeset=app_result->nodesetval;
	for(i=0;i<nodeset->nodeNr;i++)
	{ 
	  cur=nodeset->nodeTab[i];
	  cur=cur->xmlChildrenNode;
	   
	  p=xmlNewChild(cur,NULL,(const xmlChar *)"session",NULL);

	  memset(tempor,0,N);
	  sprintf(tempor,"%d",session.session_id);	     /*intת��char*/
	  q=xmlNewChild(p,NULL,(const xmlChar *)"id",(const xmlChar *)tempor);
	  
	  q=q->parent;
	  p=xmlNewChild(q,NULL,(const xmlChar *)"name",(const xmlChar *)session.user_name);
	  q=p;
	  
	  q=q->parent;
	  memset(tempor,0,N);
	  sprintf(tempor,"%d",session.access_time);	 /*intת��char*/
	  p=xmlNewChild(q,NULL,(const xmlChar *)"accTime",(const xmlChar *)tempor);
	  q=p;
	  
	  q=q->parent;
	  p=xmlNewChild(q,NULL,(const xmlChar *)"lastAccIime",(const xmlChar *)tempor);
	  q=p;
		  
	  q=q->parent; 	 
	  time_t now;
	  //fprintf(stderr,"~~~~~Add_session_Node(), pid=%d  time===%d\n", getpid(),(unsigned int)time(&now));
	  fprintf(stderr,"~~~~~Add_session_Node()\n");
	  xmlSaveFile(USER_LOG_IN,doc);	   /*�����ļ�*/
	 
    }	
	xmlXPathFreeObject(app_result);
  }
  free(tempor);
  #if 0
  {
  	
	FILE *fp;
	if((fp=fopen(USER_LOG_IN,"a+"))==NULL)		
		return;
	char str[32] = {0};
	snprintf(str, 31, "<!--pid=%d-->", getpid());
	fputs(str, fp);
	fclose(fp);
	
  }
  #endif
}


void Add_userCount(xmlDocPtr doc)
{
	xmlNodePtr cur; 				 /*����ڵ�ָ��*/
	xmlChar *value;
	xmlChar *xpath=BAD_CAST "/root";
	int i;
	int usercount;	
	char *endptr = NULL;  
	time_t now;

 	xmlXPathObjectPtr app_result=get_nodeset(doc,xpath);
 	if(app_result==NULL)
 	{
   		fprintf(stderr,"app_result is NULL.");
   		return;
 	}

 	if(app_result)
 	{
   		xmlNodeSetPtr nodeset=app_result->nodesetval;
   		for(i=0;i<nodeset->nodeNr;i++)
   		{ 
     		cur=nodeset->nodeTab[i];
	 		cur=cur->xmlChildrenNode;
	 		while(cur!=NULL)
	 		{
	   			value=xmlNodeGetContent(cur);      /*ȡ�ڵ������*/
	   			if(value!=NULL) 
	   			{	     
		 			usercount=strtoul((char *)value,&endptr,10);    /*charת��int*/ 
         			usercount+=1;                                   /*userCount��1*/		 
		 			sprintf((char *)value,"%d",usercount); 	        /*intת��char*/
		 			xmlNodeSetContent(cur,(const xmlChar *)value);  /*�޸Ľڵ�����*/
					 fprintf(stderr,"Add_userCount(xmlDocPtr doc)\n");
		 			xmlSaveFile(USER_LOG_IN,doc); 	/*�����ļ�*/
					
		 			break;
		 			xmlFree(value);
	   			}
	 		}
  		 }
   		xmlXPathFreeObject(app_result);
 	}
	#if 0
	{
		
	FILE *fp;
	if((fp=fopen(USER_LOG_IN,"a+"))==NULL)		
		return;
	char str[32] = {0};
	snprintf(str, 31, "<!--pid=%d-->", getpid());
	fputs(str, fp);
	fclose(fp);
	
  }
	#endif
}

/***********************************************/
/*
    ���ܣ����û���Ϣ�ļ�������һ���û���Ϣ
    ���룺USER_PROFILE session:Ҫ���ӵ��û���Ϣ
    ���أ���
*/
/***********************************************/

void add_user_infor(SESSION_PROFILE session)
{	
	xmlDocPtr doc;
	fprintf(stderr," add_user_infor(SESSION_PROFILE session)\n");
	doc=xmlReadFile(USER_LOG_IN,MY_ENCODING,256);   
	 
    if(doc==NULL)
    {
        fprintf(stderr,"Document not parsed successfully.");
        return;	
    } 
    ParseDoc(doc);
    Add_session_Node(doc,session);    /*��web_session.xml������session�ڵ�*/
	Add_userCount(doc);   /*web_session.xml��userCount��1*/
    xmlFreeDoc(doc);                   
}



void DelNode(xmlDocPtr doc,char *session_id)
{
	xmlNodePtr cur; 				 /*����ڵ�ָ��*/
  	xmlNodePtr tempNode;
  	xmlChar *value;
  	xmlChar *xpath=BAD_CAST "/root/session";
  	int i;
	 time_t now;
  	xmlXPathObjectPtr app_result=get_nodeset(doc,xpath);
  	if(app_result==NULL)
  	{
  		fprintf(stderr,"app_result is NULL!<br>");
		return;
  	}

  	if(app_result)
  	{
    	xmlNodeSetPtr nodeset=app_result->nodesetval;
		for(i=0;i<nodeset->nodeNr;i++)
		{ 
	  		cur=nodeset->nodeTab[i];
	  		cur=cur->xmlChildrenNode;

	  		while(cur!=NULL)
	  		{
	    		value=xmlNodeGetContent(cur);    /*ȡ�ڵ������*/
				if(value!=NULL) 
				{		  
		  			if(!strcmp((char *)value,session_id))   /*�����Ҫ�ҵĽڵ�*/
		  			{          
		    			cur=cur->parent;                 
						tempNode = cur->next;   /*�ݴ�Ҫɾ���ڵ����һ�ڵ�*/
						xmlUnlinkNode(cur);     /*�Ͽ�Ҫɾ���ڵ�*/
						xmlFreeNode(cur);       /*�ͷ�Ҫɾ���ڵ�����*/
						cur = tempNode;         /*����һ�ڵ�����*/
						break;                
		  			}
		  			else                            /*�������Ҫ�ҵĽڵ㣬ָ�����*/
		  				cur=cur->next;
		  			xmlFree(value);
				}
	  		}
			fprintf(stderr," DelNode(xmlDocPtr doc,char *session_id)\n");
	  		xmlSaveFile(USER_LOG_IN,doc);	   /*�����ļ�*/
			
		}
		xmlXPathFreeObject(app_result);
  	}
	#if 0
	{
		
	FILE *fp;
	if((fp=fopen(USER_LOG_IN,"a+"))==NULL)		
		return;
	char str[32] = {0};
	snprintf(str, 31, "<!--pid=%d-->", getpid());
	fputs(str, fp);
	fclose(fp);

  }
	#endif
}


void Del_userCount(xmlDocPtr doc)
{
	xmlNodePtr cur; 				 /*����ڵ�ָ��*/
	xmlChar *value;
	xmlChar *xpath=BAD_CAST "/root";
	int i;
	int usercount;	
	char *endptr = NULL;  
	 time_t now;
 	xmlXPathObjectPtr app_result=get_nodeset(doc,xpath);
 	if(app_result==NULL)
 	{
   		fprintf(stderr,"app_result is NULL\n");
   		return;
 	}

 	if(app_result)
 	{
   		xmlNodeSetPtr nodeset=app_result->nodesetval;
   		for(i=0;i<nodeset->nodeNr;i++)
   		{ 
     		cur=nodeset->nodeTab[i];
	 		cur=cur->xmlChildrenNode;
	 		while(cur!=NULL)
	 		{
	   			value=xmlNodeGetContent(cur);      /*ȡ�ڵ������*/
	   			if(value!=NULL) 
	   			{	     
		 			usercount=strtoul((char *)value,&endptr,10);    /*charת��int*/ 
         			usercount-=1;                                   /*userCount��1*/		 
		 			sprintf((char *)value,"%d",usercount); 	        /*intת��char*/
		 			xmlNodeSetContent(cur,(const xmlChar *)value);  /*�޸Ľڵ�����*/
					fprintf(stderr,"Del_userCount(xmlDocPtr doc)\n");
		 			xmlSaveFile(USER_LOG_IN,doc);               	/*�����ļ�*/
					//fprintf(stderr,"Del_userCount(xmlDocPtr doc)\n");
					break;
		 			xmlFree(value);
	   			}
	 		}
   		}
   		xmlXPathFreeObject(app_result);
 	}
	#if 0
	{
		
	FILE *fp;
	if((fp=fopen(USER_LOG_IN,"a+"))==NULL)		
		return;
	char str[32] = {0};
	snprintf(str, 31, "<!--pid=%d-->", getpid());
	fputs(str, fp);
	fclose(fp);
	
  }
	#endif
}

/***********************************************/
/*
    ���ܣ����û���Ϣ�ļ���ɾ��һ���û���Ϣ
    ���룺char* session_id:Ҫɾ�����û�ID
    ���أ���
*/
/***********************************************/
void del_user_infor(char* session_id)
{
	xmlDocPtr doc;
	//fprintf(stderr,"del_user_infor(char* session_id)\n");
	doc=xmlReadFile(USER_LOG_IN,MY_ENCODING,256);	
    if(doc==NULL)			 
    {
    	fprintf(stderr,"Document not parsed successfully.\n");
     	return;
    } 
   	ParseDoc(doc);
   	DelNode(doc,session_id);	 /*��session.xml��ɾ��session�ڵ�*/
	Del_userCount(doc);      /*web_session.xml��userCount��1*/
    xmlFreeDoc(doc);
}



void modLastTime(xmlDocPtr doc,char *session_id,int new_access_time)
{
	xmlNodePtr cur; 				 /*����ڵ�ָ��*/
	xmlChar *value;
	char *xpath="/root/session";		
	int i;
	char *tempor=(char *)malloc(N);
     time_t now;
 	xmlXPathObjectPtr app_result=get_nodeset(doc,(xmlChar *)xpath);
 	if(app_result==NULL)
 	{
   		fprintf(stderr,"app_result is NULL\n");
   		return;
 	}

 	if(app_result)
 	{
   		xmlNodeSetPtr nodeset=app_result->nodesetval;
   		for(i=0;i<nodeset->nodeNr;i++)
   		{ 
     		cur=nodeset->nodeTab[i];
	 		while(cur!=NULL)
	 		{	   
	   			cur=cur->xmlChildrenNode;
	   			value=xmlNodeGetContent(cur);      /*ȡ�ڵ������*/
	   			if(value!=NULL) 
	   			{
	     			if(!strcmp((char *)value,session_id))      /*�����Ҫ�ҵĽڵ�*/
	     			{        
		   				cur=cur->next;
		   				cur=cur->next;  
		   				cur=cur->next;                        /*��ָ��ָ��LastAccTime�Ľڵ�*/
						memset(tempor,0,N);
	  					sprintf(tempor,"%d",new_access_time);	 /*intת��char*/
	       				xmlNodeSetContent(cur,(const xmlChar *)tempor);  /*�޸Ľڵ�����*/		
						fprintf(stderr,"modLastTime()\n");
           				xmlSaveFile(USER_LOG_IN,doc);     /*�����ļ�*/
		   				break;                        /*����ѭ��*/
		 			}
		 			else                            /*�������Ҫ�ҵĽڵ㣬ָ�����*/
		   				cur=cur->next;
		 			xmlFree(value);
	   			}
	 		}
   		}
   		xmlXPathFreeObject(app_result);
 	}
	free(tempor);
	#if 0
	{
		
	FILE *fp;
	if((fp=fopen(USER_LOG_IN,"a+"))==NULL)		
		return;
	char str[32] = {0};
	snprintf(str, 31, "<!--pid=%d-->", getpid());
	fputs(str, fp);
	fclose(fp);
	
  }
	#endif
}




/***********************************************/
/*
    ���ܣ��޸��û���Ϣ�ļ��е���Ϣ
    ���룺char* session_id:Ҫ�޸ĵ��û�ID
                    int new_access_time:������ϵͳʱ��
    ���أ���
*/
/***********************************************/
void modify_Last_AccTime(char* session_id,int new_access_time)
{
	xmlDocPtr doc;
	fprintf(stderr,"ooo-----ooo\n");
	doc=xmlReadFile(USER_LOG_IN,MY_ENCODING,256);    /*�����ļ�*/
    if(doc==NULL)
    {
    	fprintf(stderr,"Document not parsed successfully.\n");
        return;
    } 	 
	ParseDoc(doc);
 	modLastTime(doc,session_id,new_access_time); 	
	xmlFreeDoc(doc);
}


/***********************************************/
/*
    ���ܣ�����session id�����û���Ϣ
    ���룺char* session_id:Ҫ�����û���session ID
    			int ser_type:Ҫ���ҵ���Ϣ����
    			                  1---name
    			                  2---accTime
    			                  3---LastAccTime
    ���أ�
                1:�ҵ���char* ser_result�ǲ��ҵĽ��
                0:û�ҵ�
*/
/***********************************************/
int Search_user_infor_byID(char* session_id,int ser_type,char* ser_result)
{
	xmlDocPtr doc;
	xmlNodePtr cur,tmp; 				 /*����ڵ�ָ��*/
	xmlChar *value,*result;
	int i;
	
	if(access(USER_LOG_IN,0)==-1)  /*�ļ�������*/
	{
		new_user_profile();
	}
	//fprintf(stderr,"Search_user_infor_byID()\n");
	doc=xmlReadFile(USER_LOG_IN,MY_ENCODING,256);   
  	if(doc==NULL)
  	{
		fprintf(stderr,"Document not parsed successfully.\n");
		return 0;
  	} 
	cur = xmlDocGetRootElement(doc); 
	if (cur == NULL)
	{
		xmlFreeDoc(doc);
		return -1;
	}
	if (xmlStrcmp(cur->name, (const xmlChar *) "root")) 
	{
		xmlFreeDoc(doc);
		return -1;
	}

 	cur = cur->xmlChildrenNode;	
	while(cur !=NULL)
 	{
		tmp = cur->xmlChildrenNode;
		if (!xmlStrcmp(cur->name, BAD_CAST "session"))
   		{ 
     		
	 		while(tmp !=NULL)
	 		{	   
				if ((!xmlStrcmp(tmp->name, BAD_CAST "id")))
				{
					value=xmlNodeGetContent(tmp);
					if(!strcmp((char *)value,session_id))      /*�����Ҫ�ҵĽڵ�*/
					{
	     				if((ser_type<1)||(ser_type>3))
     					{
							xmlFree(value);
							return 0;
     					}
						else
						{
     						for(i=0;i<ser_type;i++)            /*��λ��Ҫ���ҵ���Ϣ�ڵ�*/
							{	
								tmp=tmp->next;	
 							}
						}
						result=xmlNodeGetContent(tmp);      /*ȡ�ڵ������*/
							fprintf(stderr,"result--xmlNodeGetContent==%s\n",result);
	 					if(result!=NULL) 
 						{	
 							//fprintf(stderr,"result#########$$$=%s\n",result);
   							strcpy(ser_result,(char *)result);
   							xmlFree(result);
							xmlFree(value);
							return 1;
 						}
		 			}
				}
				tmp = tmp->next;
	 		}
   		}
		cur = cur->next;
 	}
	xmlFreeDoc(doc);
	return 0;
}

/***********************************************/
/*
    ���ܣ�����user name�����û���Ϣ
    ���룺char* user_name:Ҫ�����û���name
    			int ser_type:Ҫ���ҵ���Ϣ����
    			                  1---session id
    			                  2---accTime
    			                  3---LastAccTime
    ���أ�
    		  1:�ҵ���char* ser_result�ǲ��ҵĽ��
                0:û�ҵ�
*/
/***********************************************/
int Search_user_infor_byName(char* user_name,int ser_type,char* ser_result)
{
	xmlDocPtr doc;
	xmlNodePtr cur,tmp; 				 /*����ڵ�ָ��*/
	xmlChar *value,*result;
	int i;
	fprintf(stderr,"...........////..............\n");
	if(access(USER_LOG_IN,0)==-1)  /*�ļ�������*/
	{
		new_user_profile();
	}
	
	doc=xmlReadFile(USER_LOG_IN,MY_ENCODING,256);   
  	if(doc==NULL)
  	{
		fprintf(stderr,"Document not parsed successfully.\n");
		return 0;
  	} 
	cur = xmlDocGetRootElement(doc); 
	if (cur == NULL)
	{
		xmlFreeDoc(doc);
		return -1;
	}
	if (xmlStrcmp(cur->name, (const xmlChar *) "root")) 
	{
		xmlFreeDoc(doc);
		return -1;
	}
	fprintf(stderr,".........................\n");
 	cur = cur->xmlChildrenNode;	
	while(cur !=NULL)
 	{
 		fprintf(stderr,".........ddd................\n");
		tmp = cur->xmlChildrenNode;
		if (!xmlStrcmp(cur->name, BAD_CAST "session"))
   		{ 
     		fprintf(stderr,"......sss...................\n");
	 		while(tmp !=NULL)
	 		{	   
				if ((!xmlStrcmp(tmp->name, BAD_CAST "name")))
				{
					fprintf(stderr,"...........gg..............\n");
					value=xmlNodeGetContent(tmp);
					if(!strcmp((char *)value,user_name))      /*�����Ҫ�ҵĽڵ�*/
					{
						fprintf(stderr,"................kk.........\n");
	     				if((ser_type<1)||(ser_type>3))
     					{
							xmlFree(value);
							return 0;
     					}
	     				if(ser_type==1)
     					{
							tmp=tmp->prev;
     					}
						else
						{
     						for(i=0;i<ser_type-1;i++)            /*��λ��Ҫ���ҵ���Ϣ�ڵ�*/
							{	
								tmp=tmp->next;	
 							}
						}
						result=xmlNodeGetContent(tmp);      /*ȡ�ڵ������*/
						fprintf(stderr,"result===p==ddiid============%p\n",result);
	 					if(result!=NULL) 
 						{
 							fprintf(stderr,"result=====ddiid============%s\n",result);
 							//fprintf(stderr,"result==================ssss===%s\n",result);
   							strcpy(ser_result,(char *)result);
   							xmlFree(result);
							xmlFree(value);
							return 1;
 						}
		 			}
				}
				tmp = tmp->next;
	 		}
   		}
		cur = cur->next;
 	}
	xmlFreeDoc(doc);
	return 0;
}


/***********************************************/
/*
    ���ܣ���session.xml�д���һ����¼�û���Ϣ
    ���룺char *user_name:��¼���û���
    ���أ�
                1:session��û�и��û�����Ϣ�����Խ���ϵͳ��
                0:���û��Ѿ���¼�������Խ���ϵͳ��
*/
/***********************************************/

int create_user_infor(char *user_name)
{
	SESSION_PROFILE new_session;
	char ser_result[N] = {0};
	char session_id[N] = {0};
	char *endptr = NULL;
	int timeout = 0;
	int isExist,difftime,last_time;
	time_t now;
	unsigned int current_time = 0;

	int flag;  /*1:���û���2:��ʱ�û�*/
	isExist=Search_user_infor_byName(user_name,3,&ser_result);
	timeout = get_timeout_threshold();
	if(isExist==1)  /*�û���Ϣ�Ѵ���*/
	{
		current_time = (unsigned int)time(&now);
		last_time= strtoul(ser_result,&endptr,10);	  /*charת��int��10����ʮ����*/  		
		difftime=current_time-last_time;
		if(difftime>timeout)    /*���ʳ�ʱ*/
		{
			flag=2;
		}
		else
		{
			return 0;
		}
	}	
	else           /*�û���Ϣ������*/
	{
		flag=1;
	}
	if(flag==2)   /*��ʱ�û�������ɾ��������Ϣ*/
	{
		memset(session_id,0,N);
		Search_user_infor_byName(user_name,1,&session_id);
		del_user_infor(session_id);
	}
	memset(&new_session,0,sizeof(SESSION_PROFILE));
	new_session.session_id=time((time_t*)NULL);
	memset(new_session.user_name,0,N);
	strcpy(new_session.user_name,user_name);
	new_session.access_time=time((time_t*)NULL);
	new_session.last_access_time=time((time_t*)NULL);
	add_user_infor(new_session);
	return 1;
}

/******************************************/
/*
  �ж��û��Ƿ�ʱ����ʱ�˾ͷ���һ��ֵ

  2009-07-02 zhouyanmei

*/
/******************************************/
int if_user_outtime(char *user_name)
{
	char *ser_result=(char *)malloc(N);
	char *endptr = NULL;
	int isExist,difftime,last_time;
	time_t now;
	unsigned int current_time = 0;
	int flag;  /*1:���û���2:��ʱ�û�*/
	int timeout = 0;
	memset(ser_result,0,N);
	timeout = get_timeout_threshold();
	isExist=Search_user_infor_byName(user_name,3,ser_result);
	if(isExist==1)  /*�û���Ϣ�Ѵ���*/
	{
		current_time = (unsigned int)time(&now);
		last_time= strtoul(ser_result,&endptr,10);	  /*charת��int��10����ʮ����*/  		
		difftime=current_time-last_time;
		if(difftime>timeout)    /*���ʳ�ʱ*/
			flag=2;
		else
			return 0;
	}	
	else           /*�û���Ϣ������*/
		flag=1;

	free(ser_result);
	return flag;
}

///////add by tangsiqi 2009-5-25/////////////
int Input_IP_address_Check(char * ipAddress)
{
	if( ipAddress == NULL )
	{
		return INPUT_NULL;
	}

	int i;
	/*int mask;
	char * endptr;
	char * strPart1, *strPart2;
	char * strSplit = "/";*/
	char * str = NULL;
	str = ipAddress ;
	int length = strlen(str);
	if( length > WEB_IPMASK_STRING_MAXLEN || length < WEB_IPMASK_STRING_MINLEN )
	{
		return INPUT_LENGTH_ERROR;
	}
	
	for( i=0; i<length; i++ )
		{
			if( str[i] != '.'  && \
				str[i] != '/'  && \
				str[i] != '\0' && \
				( str[i] > '9' || str[i] < '0' )
			  )
			  return INPUT_FORMAT_ERROR;
		}
	#if 0       //Ϊ���MASK�Ϸ��ԣ�Ŀǰֻ���IP��ַ
	strPart1 = NULL;
	strPart1 = strtok(str, strSplit);
	strPart2 = strtok(NULL, strSplit);
	fprintf(stderr,"strPart1=%s--strPart2=%s\n",strPart1,strPart2);
	mask = strtoul(strPart2, &endptr, 0);
	if( mask < 0 || mask > 32 )
	{
		return INPUT_IP_MASK_ERROR;
	}
	
	
	if( INPUT_OK != check_ip_part(strPart1) )
	{
		return INPUT_IP_ERROR;
	}
	#endif
	//fprintf(stderr,"start ip check!\n");
	if( INPUT_OK != check_ip_part(str) )
	{
		return INPUT_IP_ERROR;
	}
	return INPUT_OK;
}


///////add by tangsiqi 2009-5-25/////////////
int Input_MAC_address_Check(char * macAddress)
{
	if( NULL == macAddress )
	{
		return INPUT_NULL;
	}
	
	char * str = NULL;
	char c;
	int i = 0 ;

	int length = strlen(macAddress);
	if( length != WEB_MAC_STRING_MINLEN )
	{
		return INPUT_LENGTH_ERROR;
	}
	
	str = macAddress;
	for(;i<length;i++) 
	{
		c = str[i];
		if((2 == i)||(5 == i)||(8 == i)||(11 == i)||(14 == i))
		{
			if((':'!=c)&&('-'!=c))
				return INPUT_FORMAT_ERROR;
		}
		else if( (c>='0'&&c<='9') ||
				(c>='A'&&c<='F')  ||
				(c>='a'&&c<='f') )
		{
			continue;
		}
		else
		{
			return INPUT_CHAR_ERROR;
		}
    }

	/////���'-'����':'�Ƿ�һ��
	if( (str[2] != str[5])||(str[2] != str[8])||(str[2] != str[11])||(str[2] != str[14])||
		(str[5] != str[8])||(str[5] != str[11])||(str[5] != str[14])||
		(str[8] != str[11])||(str[8] != str[14]) )
	{
		return INPUT_CHAR_ERROR;
	}
	
	return INPUT_OK;
}
///////add by tangsiqi 2009-5-25/////////////

int Input_User_Name_Check(char * iName)
{
	if( NULL == iName )
	{
		return INPUT_NULL;
	}
	int i = 0;
	//fprintf(stderr,"enter user name check!\n");
	char * src = iName ;
	if( src[0] == '_' || src[0] == ' ' )
	{
		return INPUT_CHAR_ERROR;
	}
	//fprintf(stderr,"head check ok!\n");
	while( src[i] != '\0')
		{
			if( (src[i] >= '0' && src[i] <= '9') 	|| 
				(src[i] >= 'a' && src[i] <= 'z') 	||
				(src[i] >= 'A' && src[i] <= 'Z') 	||
				 src[i] == '_' || src[i] == '-' 	||
				 src[i] == '@'
			  )
			{
				//fprintf(stderr,"boby check ok!\n");
				i++;
				continue;
			}
			else
			{
				return INPUT_CHAR_ERROR;
			}
		
		}
	//fprintf(stderr,"leave user name check!\n");

	return INPUT_OK;
}
///////add by tangsiqi 2009-5-25/////////////

int Input_User_Index_Check(char * iIndex)
{
	if( iIndex == NULL )
	{
		return INPUT_NULL;//NULL
	}
	
	char * src = iIndex;
	int i;
	for( i=0; src[i]!=0; i++ )
	{
		if( isspace(src[i]) )
			{
				continue;
			}
		if( !isdigit(src[i]) )
			{
				return -2;//������
			}
	}
	return INPUT_OK;
}


////////////added by tangsqi 2009-5-25 used for Get head and Tail data in every page////////
int WEB_Pagination_get_range(int pageNumForREQ, int perPage_show_data_num, int * showHead, int *showTail)
{
	if( perPage_show_data_num == 0 )
	{
		return -1; //��ҳǰ��
	}
	if( perPage_show_data_num < 0 || perPage_show_data_num < 1 )
	{
		return -2; //���벻�Ϸ�
	}
	
	*showHead = (pageNumForREQ-1)*perPage_show_data_num;
	*showTail = (pageNumForREQ)*perPage_show_data_num-1;
	return 0;
}


///////add by tangsiqi 2009-5-25/////////////
static int check_ip_part(char * src)
{
	int ipPart[4];
	char * endptr;
	char * token = NULL;
	char * ipSplit = ".";
	int i = 1;
	
	if( src == NULL )
	{
		return -1;
	}
	/*alloc temp memery to strtok to avoid  error when occurs in strtok*/
	char src_back[32];
	memset(src_back, 0, 32);
	strncpy(src_back, src, 32);

	token = strtok( src_back, ipSplit );//edit by wk
	ipPart[0] = strtoul(token, &endptr, 10);
	//fprintf(stderr,"start dot check!\n");
	if( 0 != CHECK_IP_DOT(ipPart[0]) )
	{
		return -2;
	}
	//fprintf(stderr," dot 0 check!\n");
	
	while( token != NULL && i < 4 )
	{
		token = strtok(NULL, ipSplit);
		ipPart[i] = strtoul(token, &endptr, 10);
		//fprintf(stderr," ipPart[%d]=%d !\n",i,ipPart[i]);
		if( 0 != CHECK_IP_DOT(ipPart[i]) )
		{
			return -2;
		}
		i++;
	}
	//fprintf(stderr," dot later check!\n");
	return INPUT_OK;
}

static int CHECK_IP_DOT(int a)
{
	if(a<0 || a>255)
		return INPUT_ERROR;
	else return INPUT_OK;
}
/*----------- password check ---------------------------*/
int ccgi_passwd_alive_time(int time)
{
	int ret = 0;
	int status = 0;
	char cmd[128] = {0};
	char modstr[128] = {0};
	sprintf(modstr,"'passwd alive time %d'",time);
	sprintf(cmd,"source vtysh_start.sh >/dev/null 2>&1\n"
						"vtysh -c %s",modstr);
	//sprintf(cmd,"sudo passwdmod.sh %s",modstr);
	if(strchr(cmd,';'))
	{
		return -1;
	}
	status = system(cmd);
	ret = WEXITSTATUS(status);
	//fprintf(stderr,"modstr = %s, ret = %d\n",modstr,ret);
	return ret ;
}
int ccgi_passwd_max_error(int time)
{
	int ret = 0;
	int status = 0;
	char cmd[128] = {0};
	char modstr[128] = {0};
	sprintf(modstr,"'passwd max error %d'",time);
	sprintf(cmd,"source vtysh_start.sh >/dev/null 2>&1\n"
						"vtysh -c %s",modstr);
	//sprintf(cmd,"sudo passwdmod.sh %s",modstr);
	if(strchr(cmd,';'))
	{
		return -1;
	}
	status = system(cmd);
	ret = WEXITSTATUS(status);
	return ret ;
}
int ccgi_passwd_min_length(int time)
{
	int ret = 0;
	int status = 0;
	char cmd[128] = {0};
	char modstr[128] = {0};
	sprintf(modstr,"'passwd minlength %d'",time);
	sprintf(cmd,"source vtysh_start.sh >/dev/null 2>&1\n"
						"vtysh -c %s",modstr);
	//sprintf(cmd,"sudo passwdmod.sh %s",modstr);
	if(strchr(cmd,';'))
	{
		return -1;
	}
	status = system(cmd);
	ret = WEXITSTATUS(status);
	//fprintf(stderr,"modstr = %s, ret = %d\n",modstr,ret);
	return ret ;
}
int ccgi_passwd_unrepeat(int time)
{
	int ret = 0;
	int status = 0;
	char cmd[128] = {0};
	char modstr[128] = {0};
	sprintf(modstr,"'passwd unrepeat %d'",time);
	sprintf(cmd,"source vtysh_start.sh >/dev/null 2>&1\n"
						"vtysh -c %s",modstr);
	//sprintf(cmd,"sudo passwdmod.sh %s",modstr);
	if(strchr(cmd,';'))
	{
		return -1;
	}
	status = system(cmd);
	ret = WEXITSTATUS(status);
	return ret ;
}

int ccgi_get_login_setting(int* maxdays)
{
	FILE* fp = NULL;
	char buf[512]={0};
	int getsetting = 0;

	fp = fopen(CONLOGINSETTINGFILE, "r");
	if(NULL == fp) {
		return 1;
	}
	else 
	{
		while(fgets(buf,512,fp))
		{
			if(buf[0] == '#')
				continue;
			else if(1==sscanf(buf,"PASS_MAX_DAYS   %d",maxdays))
			{
				getsetting=1;
				break;
			}
		}
		fclose(fp);
	}
	if(getsetting)
		return 0;
	else
		return 1;
	
}

int ccgi_get_pwd_err_setting(int* times)
{
	FILE* fp = NULL;
	char buf[512]={0};
	int getsetting = 0;

	fp = fopen(CONPWDERRSETTINGFILE, "r");
	if(NULL == fp) {
		
		return 1;
	}
	else 
	{
		while(fgets(buf,512,fp))
		{
			if(buf[0] == '#')
				continue;
			else if(1==sscanf(buf,"auth    required        pam_tally.so per_user deny=%d",times))
			{
				getsetting=1;
				break;
			}
		}
		fclose(fp);
	}
	if(getsetting)
		return 0;
	else
		return 1;
	
}
int ccgi_get_pwd_unrepeat_setting(int *unreplynum,int *minlen,int *strongflag)
{
	FILE* fp = NULL;
	char buf[512]={0};
	int minclass=0,difok=0;

	fp = fopen(CONPWDSYSSETTING, "r");
	
	if(NULL == fp) 
	{
		return 0;
	}
	else 
	{
	/* password   required   pam_unix.so nullok md5 remember=2  obscure min=6*/
		while(fgets(buf,512,fp))
		{
			if(buf[0] == '#')
			{
				continue;
			}
			//else if(2==sscanf(buf,"password required pam_unix.so nullok md5 remember=%d min=%d debug",&passwdunreplytimes,&passwdlen))
			else if(2==sscanf(buf,"password required pam_unix.so nullok md5 remember=%d min=%d debug",unreplynum,minlen))
			{
				continue;
			}
			else if(2==sscanf(buf,"password required pam_cracklib.so use_authtok minclass=%d difok=%d debug",&minclass,&difok))
			{
				//strongpasswd =1;
				*strongflag = 1;
			}			
		}		
	}
	fclose(fp);
	return 0;
	
}
void init_user_syslog_xml()
{		
	struct group * userlist = NULL;
	char *username=NULL;			   
	xmlDocPtr doc=NULL; 	
	xmlNodePtr root_node=NULL,node1=NULL,node2=NULL;
	int j = 0;

	doc = xmlNewDoc((const xmlChar *)"1.0");

	root_node = xmlNewNode(NULL,(const xmlChar *)"root");
	xmlDocSetRootElement(doc,root_node);
	
	userlist = getgrnam(ADMINGROUP);
	if (userlist)
	{
		for(j=0;(username=userlist->gr_mem[j])!=NULL;j++)
		{				
			node1=xmlNewChild(root_node,NULL,(const xmlChar *)"user_syslog",NULL);
			
			node2=xmlNewChild(node1,NULL,(const xmlChar *)"username",(const xmlChar *)username);

			node2=node2->parent;
			node1=xmlNewChild(node2,NULL,(const xmlChar *)"log_lever",(const xmlChar *)syslog_level[0]);/*default log_lever is debug*/
			
			node1=node1->parent;
			node2=xmlNewChild(node1,NULL,(const xmlChar *)"oper_lever",(const xmlChar *)sysoper_level[0]);/*default oper_level is none*/
		}
	}
	xmlSaveFile(USER_SYSLOG_XML,doc);
	xmlFreeDoc(doc);
}

int get_user_syslog_by_name(char *username,char *log_info,char *oper_info) 
{
	xmlDocPtr doc=NULL; 			   /*��������ĵ�ָ��*/
	xmlNodePtr cur1=NULL,cur2=NULL,tempNode=NULL;
	xmlChar *value,*tmp_value;
	xmlChar *xpath=BAD_CAST "/root/user_syslog/username";
	int j = 0;
	int retu = 0;
	system("sudo chmod 777 "USER_SYSLOG_XML"\n");
	doc=xmlReadFile(USER_SYSLOG_XML,"utf-8",256);	
	if(doc!=NULL)			 
	{				
		cur1 = xmlDocGetRootElement(doc); 
		if(cur1 != NULL)
		{
			if(!(xmlStrcmp(cur1->name, (const xmlChar *) "root")))
			{						
				xmlXPathObjectPtr app_result=get_nodeset(doc,xpath);
				if(app_result)
				{
					xmlNodeSetPtr nodeset=app_result->nodesetval;
					for(j=0;j<nodeset->nodeNr;j++)
					{
						cur2=nodeset->nodeTab[j];
						cur2=cur2->xmlChildrenNode;/*���ļ�ָ���Ƶ�user_syslog��username��*/
						value=xmlNodeGetContent(cur2);		/*ȡ�ڵ������*/
						if(value!=NULL) 
						{		  
						  if(!strcmp((char *)value,username)) /*�����Ҫ�ҵĽڵ�*/
						  { 		 
							cur2 = cur2->parent;
							tempNode = cur2->next;				/*get log_lever*/
							tmp_value=xmlNodeGetContent(tempNode);
							if(tmp_value!=NULL) 
							{
								memset(log_info,0,sizeof(log_info));
								strcpy(log_info,(char *)tmp_value);
								xmlFree(tmp_value);
							}

							tempNode = tempNode->next;			/*get oper_lever*/
							tmp_value=xmlNodeGetContent(tempNode);
							if(tmp_value!=NULL) 
							{
								memset(oper_info,0,sizeof(oper_info));
								strcpy(oper_info,(char *)tmp_value);
								xmlFree(tmp_value);
							}
							
							break;				  
						  }
						  xmlFree(value);
						}
						xmlSaveFile(USER_SYSLOG_XML,doc);	   /*�����ļ�*/
					}
					xmlXPathFreeObject(app_result);
				}
			}						
		}
		xmlFreeDoc(doc);
	} 
	return retu;
}

void add_user_syslog_by_name(char *username)
{
	xmlDocPtr doc=NULL;
	xmlNodePtr cur=NULL;				 /*����ڵ�ָ��*/
	xmlNodePtr p=NULL,q=NULL;
	xmlChar *xpath=BAD_CAST "/";

	system("sudo chmod 777 "USER_SYSLOG_XML"\n");
	doc=xmlReadFile(USER_SYSLOG_XML,"utf-8",256);   
    if(doc!=NULL)
    {
		xmlXPathObjectPtr app_result=get_nodeset(doc,xpath);		
		if(app_result)
		{
			xmlNodeSetPtr nodeset=app_result->nodesetval;
			
			cur=nodeset->nodeTab[nodeset->nodeNr-1];
			cur=cur->xmlChildrenNode;			

			p=xmlNewChild(cur,NULL,(const xmlChar *)"user_syslog",NULL);

			q=xmlNewChild(p,NULL,(const xmlChar *)"username",(const xmlChar *)username);

			q=q->parent;
			p=xmlNewChild(q,NULL,(const xmlChar *)"log_lever",(const xmlChar *)syslog_level[0]);/*default log_lever is debug*/
			q=p;

			q=q->parent;
			p=xmlNewChild(q,NULL,(const xmlChar *)"oper_lever",(const xmlChar *)sysoper_level[0]);/*default oper_level is none*/

			xmlSaveFile(USER_SYSLOG_XML,doc);	   /*�����ļ�*/
			xmlXPathFreeObject(app_result);
		}
    	xmlFreeDoc(doc);
    }
}


void del_user_syslog_by_name(char *username)
{
	xmlDocPtr doc=NULL; 			   /*��������ĵ�ָ��*/
	xmlNodePtr cur1=NULL,cur2=NULL,tempNode=NULL;
	xmlChar *value;
	xmlChar *xpath=BAD_CAST "/root/user_syslog";
	int j = 0;

	system("sudo chmod 777 "USER_SYSLOG_XML"\n");
	doc=xmlReadFile(USER_SYSLOG_XML,"utf-8",256);	
	if(doc!=NULL)			 
	{				
		cur1 = xmlDocGetRootElement(doc); 
		if(cur1 != NULL)
		{
			if(!(xmlStrcmp(cur1->name, (const xmlChar *) "root")))
			{						
				xmlXPathObjectPtr app_result=get_nodeset(doc,xpath);
				if(app_result)
				{
					xmlNodeSetPtr nodeset=app_result->nodesetval;
					for(j=0;j<nodeset->nodeNr;j++)
					{
						cur2=nodeset->nodeTab[j];
						cur2=cur2->xmlChildrenNode;/*���ļ�ָ���Ƶ�user_syslog��username��*/
						while(cur2!=NULL)
						{
							value=xmlNodeGetContent(cur2);		/*ȡ�ڵ������*/
							if(value!=NULL) 
							{		  
							  if(!strcmp((char *)value,username)) /*�����Ҫ�ҵĽڵ�*/
							  { 		 
								cur2=cur2->parent;	/*syslog*/
								tempNode = cur2->next;			/*�ݴ�Ҫɾ���ڵ����һ�ڵ�*/
								xmlUnlinkNode(cur2);			/*�Ͽ�Ҫɾ���ڵ�*/
								xmlFreeNode(cur2);				/*�ͷ�Ҫɾ���ڵ�����*/
								cur2 = tempNode;				/*����һ�ڵ�����*/
								break;				  
							  }
							  else							  /*�������Ҫ�ҵĽڵ㣬ָ�����*/
							  {
								  cur2=cur2->next;
							  }
							  xmlFree(value);
							}
						}
						xmlSaveFile(USER_SYSLOG_XML,doc);	   /*�����ļ�*/
					}
					xmlXPathFreeObject(app_result);
				}
			}						
		}
		xmlFreeDoc(doc);
	} 
}

/*return 1 :success*/
/*return 0 :fail*/
int mod_user_syslog_by_name(char *username,char *log_info,char *oper_info)
{
	int retu = 0;
	xmlDocPtr doc=NULL; 			   /*��������ĵ�ָ��*/
	xmlNodePtr cur1=NULL,cur2=NULL,tempNode=NULL;
	xmlChar *value,*tmp_value;
	xmlChar *xpath=BAD_CAST "/root/user_syslog";
	int j = 0;
	
	system("sudo chmod 777 "USER_SYSLOG_XML"\n");
	doc=xmlReadFile(USER_SYSLOG_XML,"utf-8",256);	
	if(doc!=NULL)			 
	{				
		cur1 = xmlDocGetRootElement(doc); 
		if(cur1 != NULL)
		{
			if(!(xmlStrcmp(cur1->name, (const xmlChar *) "root")))
			{						
				xmlXPathObjectPtr app_result=get_nodeset(doc,xpath);
				if(app_result)
				{
					xmlNodeSetPtr nodeset=app_result->nodesetval;
					for(j=0;j<nodeset->nodeNr;j++)
					{
						cur2=nodeset->nodeTab[j];
						while(cur2!=NULL)
						{
							cur2=cur2->xmlChildrenNode;/*���ļ�ָ���Ƶ�user_syslog��username��*/
							value=xmlNodeGetContent(cur2);		/*ȡ�ڵ������*/
							if(value!=NULL) 
							{		  
							  if(!strcmp((char *)value,username)) /*�����Ҫ�ҵĽڵ�*/
							  { 		 
								cur2 = cur2->next;
								xmlNodeSetContent(cur2,(const xmlChar *)log_info);	/*�޸Ľڵ�����*/
							
								cur2 = cur2->next;			/*get oper_lever*/
								xmlNodeSetContent(cur2,(const xmlChar *)oper_info);  /*�޸Ľڵ�����*/

								retu = 1;
								break;				  
							  }
							  else                            /*�������Ҫ�ҵĽڵ㣬ָ�����*/
							   cur2=cur2->next;
							  xmlFree(value);
							}
						}
						
						xmlSaveFile(USER_SYSLOG_XML,doc);	   /*�����ļ�*/
					}
					xmlXPathFreeObject(app_result);
				}
			}						
		}
		xmlFreeDoc(doc);
	} 
	return retu;
}
int get_timeout_threshold()
{
	FILE *fp = NULL;
	char timeout_threshold[20] = { 0 };
	int timeout = 0;
	
	if(access(TIMEOUT_THRESHOLD_CONF,0)==0)
	{
		system("sudo chmod 666 "TIMEOUT_THRESHOLD_CONF"\n");
		if(( fp = fopen( TIMEOUT_THRESHOLD_CONF ,"r")) != NULL)
		{			
			fgets(timeout_threshold,sizeof(timeout_threshold),fp);
			fclose(fp);
		}
		timeout = strtoul(timeout_threshold,0,10);
	}
	else
	{
		timeout = 180;
	}
	return timeout;
}
