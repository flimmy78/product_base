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
* ws_version_param.c
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
#include "ws_version_param.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpathInternals.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
/*#include "cgic.h"*/

#define BAD_CAST (const xmlChar *)

int get_version_param(ST_VERSION_PARAM *version, char *xml_path)
{
 xmlDocPtr pdoc = NULL;

 xmlNodePtr pcurnode = NULL;
 char *psfilename;
 
 psfilename = xml_path;
 
 memset(version, 0, sizeof(ST_VERSION_PARAM) );

 pdoc = xmlReadFile(psfilename,"utf-8",256);  //�����ļ�

  if(NULL == pdoc)
  {
     return 1;

  }  
 pcurnode = xmlDocGetRootElement(pdoc);  //�õ����ڵ�   

  //xmlChar *key;
  xmlChar *value;
  pcurnode=pcurnode->xmlChildrenNode;  //�õ��ӽڵ㼯��,����������

   while (pcurnode != NULL)   //�����ӽڵ㼯�ϣ��ҳ�����ģ�
  {     

	//sendtype 0,local upload  1,web upload  (int)
  if ((!xmlStrcmp(pcurnode->name, BAD_CAST  R_ST)))
    {   
     value = xmlNodeGetContent(pcurnode);	
	// version->sendtype=(char *)malloc(xmlStrlen(value));	
	 strcpy(version->sendtype,(char *)value);	
	 xmlFree(value); 	 
    }   
  //failcause network is unreach  (char)
  if ((!xmlStrcmp(pcurnode->name, BAD_CAST  R_FC)))
    {   
     value = xmlNodeGetContent(pcurnode);	
	// version->failcause=(char *)malloc(xmlStrlen(value));	
	 strcpy(version->failcause,(char *)value);	
	 xmlFree(value); 	 
    }   
  //filetype  1,version 2,conf 3,log  (int)
  if ((!xmlStrcmp(pcurnode->name, BAD_CAST  R_FT)))
    {   
     value = xmlNodeGetContent(pcurnode);	
	// version->filetype=(char *)malloc(xmlStrlen(value));	
	 strcpy(version->filetype,(char *)value);	
	 xmlFree(value); 	 
    }   
  //protocal 0,http 1,ftp   (int)
  if ((!xmlStrcmp(pcurnode->name, BAD_CAST  R_PRO)))
    {   
     value = xmlNodeGetContent(pcurnode);	
	// version->protocal=(char *)malloc(xmlStrlen(value));	
	 strcpy(version->protocal,(char *)value);	
	 xmlFree(value); 	 
    }   
  //routeip server ip or url  (char)
  if ((!xmlStrcmp(pcurnode->name, BAD_CAST  R_RI)))
    {   
     value = xmlNodeGetContent(pcurnode);	
	// version->routeip=(char *)malloc(xmlStrlen(value));	
	 strcpy(version->routeip,(char *)value);	
	 xmlFree(value); 	 
    }   
  //username  link username (char)
  if ((!xmlStrcmp(pcurnode->name, BAD_CAST  R_UN)))
    {   
     value = xmlNodeGetContent(pcurnode);	
	// version->username=(char *)malloc(xmlStrlen(value));	
	 strcpy(version->username,(char *)value);	
	 xmlFree(value); 	 
    }   
  //password link passwork (char)
  if ((!xmlStrcmp(pcurnode->name, BAD_CAST  R_PS)))
    {   
     value = xmlNodeGetContent(pcurnode);	
	// version->password=(char *)malloc(xmlStrlen(value));	
	 strcpy(version->password,(char *)value);	
	 xmlFree(value); 	 
    }   
  //running  run status  0,transfer 1,completed 2,fail
  if ((!xmlStrcmp(pcurnode->name, BAD_CAST  R_RUN)))
    {   
     value = xmlNodeGetContent(pcurnode);	
	// version->running=(char *)malloc(xmlStrlen(value));	
	 strcpy(version->running,(char *)value);	
	 xmlFree(value); 	 
    }   
  //routeport  server port (int)
  if ((!xmlStrcmp(pcurnode->name, BAD_CAST  R_RP)))
    {   
     value = xmlNodeGetContent(pcurnode);	
	// version->routeport=(char *)malloc(xmlStrlen(value));	
	 strcpy(version->routeport,(char *)value);	
	 xmlFree(value); 	 
    }   
  //filename  load file name (char)
  if ((!xmlStrcmp(pcurnode->name, BAD_CAST  R_FN)))
    {   
     value = xmlNodeGetContent(pcurnode);	
	// version->filename=(char *)malloc(xmlStrlen(value));	
	 strcpy(version->filename,(char *)value);	
	 xmlFree(value); 	 
    }   
  
  pcurnode = pcurnode->next;
   	} 
  
  xmlFreeDoc(pdoc);
  xmlCleanupParser();
  return 0;
   }

int set_version_param(ST_VERSION_PARAM version, char *xml_path)
{
 xmlDocPtr pdoc = NULL;

 xmlNodePtr pcurnode = NULL;
 char *psfilename;
 
 int flag=0;
 
 psfilename = xml_path;

 pdoc = xmlReadFile(psfilename,"utf-8",256);  //�����ļ�
 
  if(NULL == pdoc)
  {
     return 1;
  }

  pcurnode = xmlDocGetRootElement(pdoc);  //�õ����ڵ�    

  pcurnode = pcurnode->xmlChildrenNode;  //�õ��ӽڵ㼯��

  //xmlNodePtr propNodePtr = pcurnode;
  
  while (NULL != pcurnode)
  {			
             
        //sendtype 0,local upload  1,web upload  (int)
       if ((!xmlStrcmp(pcurnode->name, BAD_CAST  R_ST)))
         {            			
     	  xmlNodeSetContent(pcurnode, BAD_CAST  version.sendtype);                         
         }   
       //failcause network is unreach  (char)
       if ((!xmlStrcmp(pcurnode->name, BAD_CAST  R_FC)))
         {            			
     	xmlNodeSetContent(pcurnode, BAD_CAST  version.failcause);                         
         }   
       //filetype  1,version 2,conf 3,log  (int)
       if ((!xmlStrcmp(pcurnode->name, BAD_CAST  R_FT)))
         {            			
     	xmlNodeSetContent(pcurnode, BAD_CAST  version.filetype);                         
         }   
       //protocal 0,http 1,ftp   (int)
       if ((!xmlStrcmp(pcurnode->name, BAD_CAST  R_PRO)))
         {            			
     	xmlNodeSetContent(pcurnode, BAD_CAST  version.protocal);                         
         }   
       //routeip server ip or url  (char)
       if ((!xmlStrcmp(pcurnode->name, BAD_CAST  R_RI)))
         {            			
     	xmlNodeSetContent(pcurnode, BAD_CAST  version.routeip);                         
         }   
       //username  link username (char)
       if ((!xmlStrcmp(pcurnode->name, BAD_CAST  R_UN)))
         {            			
     	xmlNodeSetContent(pcurnode, BAD_CAST  version.username);                         
         }   
       //password link passwork (char)
       if ((!xmlStrcmp(pcurnode->name, BAD_CAST  R_PS)))
         {            			
     	xmlNodeSetContent(pcurnode, BAD_CAST  version.password);                         
         }   
       //running  run status  0,transfer 1,completed 2,fail
       if ((!xmlStrcmp(pcurnode->name, BAD_CAST  R_RUN)))
         {            			
     	 xmlNodeSetContent(pcurnode, BAD_CAST  version.running);                         
         }   
       //routeport  server port (int)
       if ((!xmlStrcmp(pcurnode->name, BAD_CAST  R_RP)))
         {            			
     	xmlNodeSetContent(pcurnode, BAD_CAST  version.routeport);                         
         }   
       //filename  load file name (char)
       if ((!xmlStrcmp(pcurnode->name, BAD_CAST  R_FN)))
        {            			
     	xmlNodeSetContent(pcurnode, BAD_CAST  version.filename);                         
        }   
    		 
    pcurnode = pcurnode->next; 
                         
  }	  
	xmlSaveFile(xml_path,pdoc);  //����xml�ļ�
	return flag; 
}

//����һ���ڵ��ֵ
int set_one_node(char *xml_path,char *node,char *newc)
{
 xmlDocPtr pdoc = NULL;
 xmlNodePtr pcurnode = NULL;
 char *psfilename; 
 int flag=0; 
 psfilename = xml_path;
 
 pdoc = xmlReadFile(psfilename,"utf-8",256);  //�����ļ�
 
  if(NULL == pdoc)
  {
     return 1;
  }

  pcurnode = xmlDocGetRootElement(pdoc);  //�õ����ڵ�    

  pcurnode = pcurnode->xmlChildrenNode;  //�õ��ӽڵ㼯��

  //xmlNodePtr propNodePtr = pcurnode;
  
  while (NULL != pcurnode)
  {	             
       if ((!xmlStrcmp(pcurnode->name, BAD_CAST  node)))
         {            			
     	  xmlNodeSetContent(pcurnode, BAD_CAST  newc);                         
         }     		 
    pcurnode = pcurnode->next;                         
  }	  
	xmlSaveFile(xml_path,pdoc);  //����xml�ļ�
	return flag; 
}

//0: �ϴ���ftp������  1,��ftp������������
int file_ftp(char *ip,char *username,char *passwd,char *dir,char *filename,int type)
{
int iRet,status;
char *cmd;
cmd=(char *)malloc(128);

if(type==0)
sprintf(cmd,"ftp_put.sh %s %s %s %s %s > /dev/null",ip,username,passwd,dir,filename);
else if(type==1)
sprintf(cmd,"ftp_get.sh %s %s %s %s %s > /dev/null",ip,username,passwd,dir,filename);
else
iRet=-1;

status = system(cmd);
iRet = WEXITSTATUS(status);
free(cmd);
return iRet;
}

int add_conf_node(char *fpath,char * node_name,char * value)
{
 xmlDocPtr pdoc = NULL;

 xmlNodePtr pcurnode = NULL; 

 char *psfilename;
 
 psfilename = fpath;

 pdoc = xmlReadFile(psfilename,"utf-8",256);  //�����ļ�
 
  if(NULL == pdoc)
  {
     return 1;
  }

  pcurnode = xmlDocGetRootElement(pdoc);  //�õ����ڵ�   
  
   //root ����ӣ��ɹ�
   
   xmlNodePtr node = xmlNewNode(NULL,BAD_CAST node_name);    

   xmlAddChild(pcurnode,node);

   xmlNodePtr content1 = xmlNewText(BAD_CAST value);

   xmlAddChild(node,content1);
 
   xmlSaveFile(fpath,pdoc);  //����xml�ļ�

	return 0;
}

/*ɾ���ڵ�*/
int del_conf_node(char *fpath,char *node_name)
{
 xmlDocPtr pdoc = NULL;

 xmlNodePtr pcurnode = NULL;
 char *psfilename;
  
 psfilename = fpath;

 pdoc = xmlReadFile(psfilename,"utf-8",256);  //�����ļ�
 
  if(NULL == pdoc)
  {
     return 1;
  }

  pcurnode = xmlDocGetRootElement(pdoc);  //�õ����ڵ�    

  pcurnode = pcurnode->xmlChildrenNode;  //�õ��ӽڵ㼯��

  while (NULL != pcurnode)
  {			
                        
     if (!xmlStrcmp(pcurnode->name, BAD_CAST node_name))   //�ҵ� root ��һ���ӽڵ�             
      {
				
           xmlNodePtr tempNode;				  		   
				
           tempNode = pcurnode->next;  // ͬ��Ľڵ�
            
           xmlUnlinkNode(pcurnode);

           xmlFreeNode(pcurnode);

           pcurnode= tempNode;			   
		 
           continue;           
			
     }        
			 pcurnode = pcurnode->next; 
                    
  }	  
	xmlSaveFile(fpath,pdoc);  //����xml�ļ�
	
	return 0;
}

/*find conf node and get the node content*/
int find_conf_node(char * fpath,char * node_name,char *outkey)
{
 xmlDocPtr pdoc = NULL;
 xmlNodePtr pcurnode = NULL;
 char *psfilename;

 int flag=-1;
  
 psfilename = fpath;
 pdoc = xmlReadFile(psfilename,"utf-8",256);  //�����ļ�
 
  if(NULL == pdoc)
  {
     return -1;
  }

  pcurnode = xmlDocGetRootElement(pdoc);  //�õ����ڵ�    

  pcurnode = pcurnode->xmlChildrenNode;  //�õ��ӽڵ㼯��

  xmlChar *value;	
 
      while (NULL != pcurnode)
      {			

             if (!xmlStrcmp(pcurnode->name, BAD_CAST node_name))   //�ҵ�  ��� root ��һ���ӽڵ�             
             {             
    			 
             	 value = xmlNodeGetContent(pcurnode);    
    			 strcpy(outkey,(char *)value);	
    			 flag=0;
                 xmlFree(value);
    		
             }  
        pcurnode = pcurnode->next; 
     }	  
	  
	xmlSaveFile(fpath,pdoc);  //����xml�ļ�
	return flag;
}

/*get one file bytes sum*/
int get_file_bytesum(char *fpath)
{
	  unsigned   long   sum=0;   
	  FILE *fp=fopen(fpath,"rb");
	  if(fp) 
	  {   
	      for(sum=0;!feof(fp);sum+=fgetc(fp));   
	      fclose(fp);   
	  }  
	  return sum;
}

//test 

//add xml node , property
int add_mibs_node_attr_z(char *fpath,char * node_name,char *attribute,char *ruler)
{
	 xmlDocPtr pdoc = NULL;

	 xmlNodePtr pcurnode_f = NULL; 
	 xmlNodePtr pcurnode = NULL; 

	 char *psfilename;
 
     psfilename = fpath;


	 int flag=0;

	 pdoc = xmlReadFile(psfilename,"utf-8",256);  //�����ļ�
	 
	  if(NULL == pdoc)
	  {
	     return 1;
	  }

	  pcurnode_f = xmlDocGetRootElement(pdoc);  //�õ����ڵ�   

	  pcurnode = pcurnode_f->xmlChildrenNode;  //�õ��ӽڵ㼯��

	  while (NULL != pcurnode)
	  {			
	                        
	          if (!xmlStrcmp(pcurnode->name, BAD_CAST node_name))   //�ҵ�  root ��һ���ӽڵ�             
	          {
					xmlChar *value;
					value = xmlGetProp(pcurnode, (const xmlChar *)attribute);
					if(strcmp(ruler,(char*)value)==0)
					{
							flag = 1;
					}
					xmlFree(value);
			  }        
			 pcurnode = pcurnode->next; 
	                    
	  }

//root��Ϊ�սڵ��ʱ�򣬾ʹ���һ����Ľڵ㣬����������
    if(flag==0)
	{
		#if 1
	    xmlNodePtr node = xmlNewNode(NULL, BAD_CAST "pid");
		xmlSetProp(node, BAD_CAST attribute, BAD_CAST ruler);
	   
		 /*
		xmlNewChild(node, NULL, BAD_CAST "vrid",BAD_CAST vrrpid);
		xmlNewChild(node, NULL, BAD_CAST "priority",BAD_CAST priority); 
		xmlNewChild(node, NULL, BAD_CAST "virtual",BAD_CAST virtual);
		xmlNewChild(node, NULL, BAD_CAST "ip",BAD_CAST ip);
		*/
		xmlAddChild(pcurnode_f, node);
		 #endif

	}
	
    xmlSaveFile(fpath,pdoc);  //����xml�ļ�

	return flag;
}


////////////////////
//mod xml node
int mod_mibs_node_z(char * fpath,char * node_name,char *attribute,char *ruler,char * content,char *newc)
{
		xmlDocPtr pdoc = NULL;

		xmlNodePtr pcurnode = NULL;

		xmlNodePtr design_node = NULL;

		pdoc = xmlReadFile(fpath,"utf-8",256);  //�����ļ�

		if(NULL == pdoc)
		{
			return 1;
		}

        int if_second_flag=0,if_second_null=0;
		
		pcurnode = xmlDocGetRootElement(pdoc);  //�õ����ڵ�    

		pcurnode = pcurnode->xmlChildrenNode;  //�õ��ӽڵ㼯��

		while (NULL != pcurnode)
	    {			

			if (!xmlStrcmp(pcurnode->name, BAD_CAST node_name))   //�ҵ� des ��� root ��һ���ӽڵ�             
			{
					
					xmlChar* szAttr = xmlGetProp(pcurnode,BAD_CAST attribute);  //ȡ������ֵ     

					if(!xmlStrcmp(szAttr,BAD_CAST ruler))
					{		
						//�˴���������һ���Ľڵ�,���û����һ���Ľڵ�
						xmlNodePtr childPtr = pcurnode; 
						childPtr=childPtr->children;
					
						while(childPtr !=NULL)
						{	 
						    if_second_null=1;
							//������ڵ���޸ģ�û�оʹ���
							if ((!xmlStrcmp(childPtr->name, BAD_CAST content)))
							{
 								xmlNodeSetContent(childPtr, BAD_CAST  newc); 
								if_second_flag=0;
								break;
								 
							}
							else
							{
							    if_second_flag=1;
								design_node=pcurnode;
								
								
							}

							childPtr = childPtr->next;
						}
						//��������ڵ��ǿյĻ�
						if(if_second_null == 0)
						{
                            if_second_flag=1;
							design_node=pcurnode;
						}
					}

					xmlFree(szAttr);
			}        
			pcurnode = pcurnode->next; 

	    }	  
	    /*���Ϊ�ջ�����û�д˶����ڵ㣬��Ҫ���д���,�����Ǵ������ɹ�
	     pcurnode �ӽڵ�ļ���*/
		if(if_second_flag==1)
		{

		    //  fprintf(cgiOut,"second node is null <br>");			  

			   xmlNodePtr node = xmlNewNode(NULL,BAD_CAST content);    

               xmlAddChild(design_node,node);
                     
               xmlNodePtr content1 = xmlNewText(BAD_CAST newc);
                     
               xmlAddChild(node,content1);

			   design_node=NULL;
	
	    }
		 
		xmlSaveFile(fpath,pdoc);  //����xml�ļ�
		
		//return if_second_null;
		return if_second_flag;
}



//create xml for mibs
int create_mibs_xml_z(char *xmlpath)
{
	xmlDocPtr doc = NULL;			/* document pointer */	
	xmlNodePtr root_node = NULL;
	
	doc = xmlNewDoc(BAD_CAST "1.0");
	root_node = xmlNewNode(NULL, BAD_CAST "root" );
	xmlDocSetRootElement(doc, root_node);

	xmlSaveFormatFileEnc(xmlpath, doc, "UTF-8", 1);
	/*free the document */
	xmlFreeDoc(doc);
	xmlCleanupParser();
	xmlMemoryDump();	  //debug memory for regression tests
	
	return 0;	
}


//get xml node signle
int get_mibs_node_z(char * fpath,char * node_name,char *attribute,char *ruler,char * content,char *logkey)
{
	xmlDocPtr pdoc = NULL;

	xmlNodePtr pcurnode = NULL;
	char *psfilename;

	psfilename = fpath;

	pdoc = xmlReadFile(psfilename,"utf-8",256);  //�����ļ�

	if(NULL == pdoc)
	{
		return 1;
	}

	pcurnode = xmlDocGetRootElement(pdoc);  //�õ����ڵ�    

	pcurnode = pcurnode->xmlChildrenNode;  //�õ��ӽڵ㼯��

	xmlNodePtr propNodePtr = pcurnode;

	while (NULL != pcurnode)
	{			
		if (!xmlStrcmp(pcurnode->name, BAD_CAST node_name))   //�ҵ� des ��� root ��һ���ӽڵ�             
		{
		    propNodePtr = pcurnode;	
			xmlChar* szAttr = xmlGetProp(propNodePtr,BAD_CAST attribute);  //ȡ������ֵ     

			if(!xmlStrcmp(szAttr,BAD_CAST ruler))  
			{	

				//�˴���������һ���Ľڵ�
				xmlNodePtr childPtr = propNodePtr; 
				childPtr=childPtr->children;
				xmlChar *value;

				while(childPtr !=NULL)
				{	 
					if ((!xmlStrcmp(childPtr->name, BAD_CAST content)))
					{
						value = xmlNodeGetContent(childPtr);        //ȡ�� ���ӽڵ��ֵ  	
						strcpy(logkey,(char *)value);			 
						xmlFree(value);
					}

					childPtr = childPtr->next;
				}
			}
			xmlFree(szAttr);
		}        
		pcurnode = pcurnode->next; 
	}	  
	xmlSaveFile(fpath,pdoc);  //����xml�ļ�
	return 0;
}

/*get node xml struct *head*/
