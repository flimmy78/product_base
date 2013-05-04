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
* capture.c
*
*
* CREATOR:
* autelan.software.Network Dep. team
*
* DESCRIPTION:
*
*
*
*******************************************************************************/

#include "ws_p3_server.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpathInternals.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include "cgic.h"




/*��ȡpppoe��xml�ļ���Ϣ*/
int read_p3_xml(char * name, ST_P3_ALL *sysall)
{
	xmlDocPtr pdoc = NULL;

	xmlNodePtr pcurnode = NULL;
	char *psfilename;

	psfilename = name;

	memset(sysall, 0, sizeof(ST_P3_ALL) );

	pdoc = xmlReadFile(psfilename,"utf-8",256);  //�����ļ�

	if(NULL == pdoc)
	{

		//fprintf(cgiOut,"error: open file %s" , psfilename);
		return 1;

	}  
	pcurnode = xmlDocGetRootElement(pdoc);  //�õ����ڵ�   

	xmlChar *value;
	xmlChar *content;
	int i=0,n=0,p=0,j=0;
	pcurnode=pcurnode->xmlChildrenNode;  //�õ��ӽڵ㼯��,����������

	while (pcurnode != NULL)   //�����ӽڵ㼯�ϣ��ҳ�����ģ�
	{     


		//radius 
		if ((!xmlStrcmp(pcurnode->name, BAD_CAST  P3_RADIUS)))
		{    

			xmlNodePtr testnode;

			testnode=pcurnode;	 
			testnode=testnode->children;

			while(testnode !=NULL)
			{	 

				//radius����serverip���Ե�        
				if ((!xmlStrcmp(testnode->name, BAD_CAST  P3_SERIP)))   
				{
					value = xmlNodeGetContent(testnode);	
					strcpy(sysall->s_radius.serip,(char *)value);	
					xmlFree(value);
				}	    	 

				//radius����password���Ե�    		
				if ((!xmlStrcmp(testnode->name, BAD_CAST P3_PWD)))   
				{
					value = xmlNodeGetContent(testnode);
					strcpy(sysall->s_radius.passwd,(char *)value);	
					xmlFree(value);
				}	

				//radius����auth���Ե�    		
				if ((!xmlStrcmp(testnode->name, BAD_CAST P3_AUTH)))   
				{
					value = xmlNodeGetContent(testnode);
					strcpy(sysall->s_radius.auth,(char *)value);	
					xmlFree(value);
				}	

				//radius����auport ���Ե�  
				if ((!xmlStrcmp(testnode->name, BAD_CAST P3_AUTH_PORT)))   
				{
					value = xmlNodeGetContent(testnode);
					strcpy(sysall->s_radius.auport,(char *)value);	
					xmlFree(value);
				}	

				//radius����acct���Ե�    		
				if ((!xmlStrcmp(testnode->name, BAD_CAST P3_ACCT)))   
				{
					value = xmlNodeGetContent(testnode);
					strcpy(sysall->s_radius.acct,(char *)value);	
					xmlFree(value);
				}
				//radius����acport���Ե�    		
				if ((!xmlStrcmp(testnode->name, BAD_CAST P3_ACCT_PORT)))   
				{
					value = xmlNodeGetContent(testnode);
					strcpy(sysall->s_radius.acport,(char *)value);	
					xmlFree(value);
				}	


				//radius����default���Ե�    	ʣ���
				if ((!xmlStrcmp(testnode->name, BAD_CAST P3_DEF)))   
				{
					value = xmlNodeGetContent(testnode);
					strcpy(sysall->s_radius.def,(char *)value);	
					xmlFree(value);
				}	

				//radius����log���Ե�    	
				if ((!xmlStrcmp(testnode->name, BAD_CAST P3_LOGIN)))   
				{
					value = xmlNodeGetContent(testnode);
					strcpy(sysall->s_radius.log,(char *)value);	
					xmlFree(value);
				}	

				//radius����dict���Ե�    	
				if ((!xmlStrcmp(testnode->name, BAD_CAST P3_DIC)))   
				{
					value = xmlNodeGetContent(testnode);
					strcpy(sysall->s_radius.dict,(char *)value);	
					xmlFree(value);
				}	

				//radius����ser���Ե�    	
				if ((!xmlStrcmp(testnode->name, BAD_CAST P3_SER)))   
				{
					value = xmlNodeGetContent(testnode);
					strcpy(sysall->s_radius.ser,(char *)value);	
					xmlFree(value);
				}	


				testnode = testnode->next;	  
			}	   
			n++;
		}  

		////////////////////////////////////////////////////////////////////////////////


		//interface  �����
		if ((!xmlStrcmp(pcurnode->name, BAD_CAST P3_INF)))
		{    

			xmlNodePtr testnode;

			testnode=pcurnode;

			testnode=testnode->children;
			while(testnode !=NULL)
			{	 

				//interface����max���Ե�

				if ((!xmlStrcmp(testnode->name, BAD_CAST  P3_MAX)))   
				{
					value = xmlNodeGetContent(testnode);	
					strcpy(sysall->s_inf.max,(char *)value);	
					xmlFree(value);
				}	

				//interface����base���Ե�

				if ((!xmlStrcmp(testnode->name, BAD_CAST P3_BASE)))  
				{
					value = xmlNodeGetContent(testnode);
					strcpy(sysall->s_inf.base,(char *)value);	
					xmlFree(value);
				}	

				//interface����myip���Ե�

				if ((!xmlStrcmp(testnode->name, BAD_CAST P3_MYIP)))  
				{
					value = xmlNodeGetContent(testnode);
					strcpy(sysall->s_inf.myip,(char *)value);	
					xmlFree(value);
				}	

				//interface����p3if(port)���Ե�

				if ((!xmlStrcmp(testnode->name, BAD_CAST P3_PORT)))  
				{
					value = xmlNodeGetContent(testnode);
					strcpy(sysall->s_inf.port,(char *)value);	
					xmlFree(value);
				}	
				testnode = testnode->next;	  
			}	   
			p++;
		}  


		////////////////////////////////////////////////////////////////////////////////

		//dns  �����
		if ((!xmlStrcmp(pcurnode->name, BAD_CAST P3_DNS)))
		{    

			xmlNodePtr testnode;

			testnode=pcurnode;

			testnode=testnode->children;
			while(testnode !=NULL)
			{	 

				//dns����hostip���Ե�
				if ((!xmlStrcmp(testnode->name, BAD_CAST  P3_HIP)))   
				{
					value = xmlNodeGetContent(testnode);	
					strcpy(sysall->s_dns.hostip,(char *)value);	
					xmlFree(value);
				}

				//dns����backip���Ե�
				if ((!xmlStrcmp(testnode->name, BAD_CAST  P3_BIP))) 
				{
					content = xmlNodeGetContent(testnode);		 
					strcpy(sysall->s_dns.backip,(char *)value);

					xmlFree(content);
				}

				//dns����def���Ե�
				if ((!xmlStrcmp(testnode->name, BAD_CAST  P3_DEFAULT))) 
				{
					content = xmlNodeGetContent(testnode);		 
					strcpy(sysall->s_dns.defu,(char *)value);

					xmlFree(content);
				}

				//dns����mask���Ե�
				if ((!xmlStrcmp(testnode->name, BAD_CAST  P3_MASK))) 
				{
					content = xmlNodeGetContent(testnode);		 
					strcpy(sysall->s_dns.mask,(char *)value);

					xmlFree(content);
				}

				//dns����lcp���Ե�
				if ((!xmlStrcmp(testnode->name, BAD_CAST  P3_LCP))) 
				{
					content = xmlNodeGetContent(testnode);		 
					strcpy(sysall->s_dns.lcp,(char *)value);

					xmlFree(content);
				}

				//dns����logfile���Ե�
				if ((!xmlStrcmp(testnode->name, BAD_CAST  P3_LFILE))) 
				{
					content = xmlNodeGetContent(testnode);		 
					strcpy(sysall->s_dns.logfile,(char *)value);

					xmlFree(content);
				}

				//dns����plugin���Ե�
				if ((!xmlStrcmp(testnode->name, BAD_CAST  P3_PLUG))) 
				{
					content = xmlNodeGetContent(testnode);		 
					strcpy(sysall->s_dns.plugin,(char *)value);

					xmlFree(content);
				}

				testnode = testnode->next;

			}	    
			i++;
		}  


		////////////////////////////////////////////////////////////////////////////////
		//host 
		if ((!xmlStrcmp(pcurnode->name, BAD_CAST  P3_HOST)))
		{    

			xmlNodePtr testnode;

			testnode=pcurnode;	 
			testnode=testnode->children;

			while(testnode !=NULL)
			{	 

				//host����hostdef���Ե�        
				if ((!xmlStrcmp(testnode->name, BAD_CAST  P3_HOSTDEF)))   
				{
					value = xmlNodeGetContent(testnode);	
					strcpy(sysall->s_host.hostdef,(char *)value);	
					xmlFree(value);
				}	    	 

				//host����hostip���Ե�    		
				if ((!xmlStrcmp(testnode->name, BAD_CAST P3_HOSTIP)))   
				{
					value = xmlNodeGetContent(testnode);
					strcpy(sysall->s_host.hostip,(char *)value);	
					xmlFree(value);
				}	

				//host����hostname���Ե�    		
				if ((!xmlStrcmp(testnode->name, BAD_CAST P3_HOSTNAME)))   
				{
					value = xmlNodeGetContent(testnode);
					strcpy(sysall->s_host.hostname,(char *)value);	
					xmlFree(value);
				}	

				testnode = testnode->next;	  
			}	  
			j++;
		}  

		////////////////////////////////////////////////////////////////////////////////

		pcurnode = pcurnode->next;
	} 
	sysall->dnum=i;
	sysall->ifnum=p;
	sysall->rnum=n;
	sysall->hnum=j;
	xmlFreeDoc(pdoc);
	xmlCleanupParser();
	return 0;
}



/*д�뵽��ͬ�������ļ���ȥ*/
int write_p3_conf( ST_P3_ALL *sysall) 
{

	FILE * fp;	
	char content[2000], temp[256];
	//int i;

	// radius�������ļ�

	if(( fp = fopen(P3_SERVER,"w+"))==NULL)
	{
		return -1;
	}
	else
	{

		memset(content,0,1024);			
		sprintf(content , "%s  %s\n",sysall->s_radius.serip,sysall->s_radius.passwd);			
		fwrite(content,strlen(content),1,fp);	
		fclose(fp);
	}	
	///////////////////////////////////////////////////////////
	if(( fp = fopen(P3_RCONF,"w+"))==NULL)
	{
		return -2;
	}
	else
	{

		memset(content,0,1024);	

		memset(temp,0,256);
		sprintf(temp , "%s",sysall->s_radius.log);			
		strcat(content,temp);

		memset(temp,0,256);
		sprintf(temp , "authserver %s:%s\n",sysall->s_radius.auth,sysall->s_radius.auport);			
		strcat(content,temp);

		memset(temp,0,256);
		sprintf(temp , "acctserver %s:%s\n",sysall->s_radius.acct,sysall->s_radius.acport);	
		strcat(content,temp);

		memset(temp,0,256);		
		sprintf(temp , "%s\n",sysall->s_radius.dict);			
		strcat(content,temp);

		memset(temp,0,256);		
		sprintf(temp , "%s\n",sysall->s_radius.ser);		
		strcat(content,temp);

		memset(temp,0,256);
		sprintf(temp , "%s",sysall->s_radius.def);	
		strcat(content,temp);

		fwrite(content,strlen(content),1,fp);	
		fclose(fp);
	}	
	///////////////////////////////////////////////////////////


	// interface and ip ����  .sh�ļ�

	system("sudo chmod 666 /etc/init.d/pppoe-server.sh");
	if(( fp = fopen(P3_SERSH,"w+"))==NULL)
	{
		return -3;
	}
	else
	{

		memset(content,0,1024);	

		memset(temp,0,256);
		sprintf(temp , "%s\n","#!/bin/bash");
		strcat(content,temp);

		memset(temp,0,256);
		sprintf(temp , "MAX=%s \n",sysall->s_inf.max);	
		strcat(content,temp);

		memset(temp,0,256);
		sprintf(temp , "BASE=%s \n",sysall->s_inf.base);
		strcat(content,temp);


		memset(temp,0,256);
		sprintf(temp , "MYIP=%s \n",sysall->s_inf.myip);
		strcat(content,temp);

		memset(temp,0,256);
		sprintf(temp , "PPPOEIF=%s \n",sysall->s_inf.port);	
		strcat(content,temp);

		memset(temp,0,256);
		sprintf(temp , "%s \n","/usr/sbin/pppoe-server -T 60 -I $PPPOEIF -N $MAX -S ax7 -L $MYIP -R $BASE");
		strcat(content,temp);

		fwrite(content,strlen(content),1,fp);	
		fclose(fp);

		system("sudo chmod 755 /etc/init.d/pppoe-server.sh");  //�޸�Ϊ��ִ�е�Ȩ��
	}	
	///////////////////////////////////////////////////////////    

	//��ȡ�ṹ����������д�뵽�ļ���ȥ  dns �������ļ�	

	if(( fp = fopen(P3_OPTION,"w+"))==NULL)
	{
		return -4;
	}
	else
	{

		memset(content,0,1024);	
		memset(temp,0,256);
		sprintf(temp , "ms-dns %s\n",sysall->s_dns.hostip);	
		strcat(content,temp);

		memset(temp,0,256);
		sprintf(temp , "ms-dns %s\n",sysall->s_dns.backip);	
		strcat(content,temp);

		memset(temp,0,256);
		sprintf(temp , "%s\n",sysall->s_dns.defu);	
		strcat(content,temp);

		memset(temp,0,256);
		sprintf(temp , "%s\n",sysall->s_dns.mask);	
		strcat(content,temp);

		memset(temp,0,256);
		sprintf(temp , "%s\n",sysall->s_dns.lcp);	
		strcat(content,temp);

		memset(temp,0,256);
		sprintf(temp , "logfile  %s\n",sysall->s_dns.logfile);	
		strcat(content,temp);

		memset(temp,0,256);
		sprintf(temp , "%s\n",sysall->s_dns.plugin);	
		strcat(content,temp);


		fwrite(content,strlen(content),1,fp);	
		fclose(fp);
	}	

	///////////////////////////////////////////////////////////    

	//��ȡ�ṹ����������д�뵽�ļ���ȥ  host �������ļ�	

	if(( fp = fopen(P3_HP,"w+"))==NULL)
	{
		return -5;
	}
	else
	{

		memset(content,0,1024);	
		memset(temp,0,256);
		sprintf(temp , "%s\n",sysall->s_host.hostdef);	
		strcat(content,temp);

		memset(temp,0,256);
		sprintf(temp , "%s %s\n",sysall->s_host.hostip,sysall->s_host.hostname);	
		strcat(content,temp);

		fwrite(content,strlen(content),1,fp);	
		fclose(fp);
	}	
	return 0;
}





/*ɾ��ָ���ڵ㣬ָ�����ԵĽڵ�*/
int p3_del(char *fpath,char *node_name,char *attribute,char *key)
{
	xmlDocPtr pdoc = NULL;

	xmlNodePtr pcurnode = NULL;
	char *psfilename;

	psfilename = fpath;

	pdoc = xmlReadFile(psfilename,"utf-8",256);  //�����ļ�

	if(NULL == pdoc)
	{
		//fprintf(cgiOut,"error: open file %s" , psfilename);
		return 1;
	}

	pcurnode = xmlDocGetRootElement(pdoc);  //�õ����ڵ�    

	pcurnode = pcurnode->xmlChildrenNode;  //�õ��ӽڵ㼯��

	while (NULL != pcurnode)
	{			

		if (!xmlStrcmp(pcurnode->name, BAD_CAST node_name))   //�ҵ� rest ��� root ��һ���ӽڵ�             
		{

			if (xmlHasProp(pcurnode,BAD_CAST attribute))
			{     



				if (!xmlStrcmp(pcurnode->name, BAD_CAST node_name))   //�ҵ� des ��� root ��һ���ӽڵ�des

				{            

					xmlNodePtr tempNode;				  		   

					tempNode = pcurnode->next;  // ͬ��Ľڵ�

					xmlUnlinkNode(pcurnode);

					xmlFreeNode(pcurnode);

					pcurnode= tempNode;			   

					continue;

				} 	 

			}
		}        
		pcurnode = pcurnode->next; 

	}	  
	xmlSaveFile(fpath,pdoc);  //����xml�ļ�
	return 0;
}


//�鿴ָ���ڵ��ֵ
/*find designed node һ���ڵ�������������������ڵ㣬���� */

int find_p3_node(char * fpath,char * node_name,char * content,char *logkey)
{
	xmlDocPtr pdoc = NULL;

	xmlNodePtr pcurnode = NULL;
	char *psfilename;

	int flag=-1;

	psfilename = fpath;

	pdoc = xmlReadFile(psfilename,"utf-8",256);  //�����ļ�

	if(NULL == pdoc)
	{
		//fprintf(cgiOut,"error: open file %s" , psfilename);
		return 1;
	}

	pcurnode = xmlDocGetRootElement(pdoc);  //�õ����ڵ�    

	pcurnode = pcurnode->xmlChildrenNode;  //�õ��ӽڵ㼯��

	xmlNodePtr propNodePtr = pcurnode;


	while (NULL != pcurnode)
	{		                        
		if (!xmlStrcmp(pcurnode->name, BAD_CAST node_name))   //root ��һ���ӽڵ�             
		{          
			propNodePtr = pcurnode;		

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
					flag=0;
					xmlFree(value);
				}         	 
				childPtr = childPtr->next;
			}		
		} 	 
		pcurnode = pcurnode->next;                     
	}	  
	xmlSaveFile(fpath,pdoc);  //����xml�ļ�
	return flag;
}


//�޸�ָ���ڵ��ֵ
int mod_p3_node(char * fpath,char * node_name,char * content,char *newc)
{
	xmlDocPtr pdoc = NULL;

	xmlNodePtr pcurnode = NULL;
	char *psfilename;

	psfilename = fpath;

	pdoc = xmlReadFile(psfilename,"utf-8",256);  //�����ļ�

	if(NULL == pdoc)
	{
		//fprintf(cgiOut,"error: open file %s" , psfilename);
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
			//�˴���������һ���Ľڵ�
			xmlNodePtr childPtr = propNodePtr; 
			childPtr=childPtr->children;
			xmlChar *value;

			while(childPtr !=NULL)
			{	 
				if ((!xmlStrcmp(childPtr->name, BAD_CAST content)))
				{
					value = xmlNodeGetContent(childPtr);         	
					xmlNodeSetContent(childPtr, BAD_CAST  newc); 
					xmlFree(value);
				}             	 
				childPtr = childPtr->next;
			}                    
		}        
		pcurnode = pcurnode->next; 

	}	  
	xmlSaveFile(fpath,pdoc);  //����xml�ļ�
	return 0;
}

/* check whether ppppoe service is enable*/
int ser_p3_enbale(){

	char status[10] = {0};
	find_p3_node(P3_XML_FPATH, P3_INF, P3_STATUS,status);
	if(!strcmp(status,"1")){
		return 1;
	}
	else{
		return 0;
	}	
}

/* check whether /opt/option/pppoe_option is exsited*/
void file_p3_exsit(){
	char cmd[128] = {0};
	if(access(P3_XML_FPATH,0) != 0){
		memset(cmd,0,128);
		sprintf(cmd,"sudo cp %s %s",P3_CONFXMLT,P3_XML_FPATH);
		system(cmd);
	}
	else{
		memset(cmd,0,128);
		sprintf(cmd,"sudo chmod 777 %s",P3_XML_FPATH);
		system(cmd);	
	 }
}
