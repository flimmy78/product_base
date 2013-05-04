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
* wp_md5_check.c
*
*
* CREATOR:
* autelan.software.Network Dep. team
*liuyu@autelan.com
*
* DESCRIPTION:
* system function for version file enable
*
*
*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cgic.h"
#include <string.h>
#include <stdlib.h>
#include "ws_err.h"
#include "ws_usrinfo.h"
#include "ws_ec.h"
#include <fcntl.h>
#include <sys/wait.h>


int sor_md5img(const char* filename)
{
	char result_str[64];
	char cmdstr[128] = {0};
	int ret,lenth=0;
	FILE *fp = 	NULL;
	
	sprintf(cmdstr,"sor.sh imgmd5 %s 120",filename);
	fprintf(stderr,"liuyu_99=%s\n",cmdstr);
	fp = popen( cmdstr, "r" );
	if(fp)
	{
		fgets( result_str, 128, fp );
		fprintf(stderr,"result_99=%s\n",result_str);
		ret = pclose(fp);
		
		switch (WEXITSTATUS(ret)) {
			case 0:
				ShowAlert(result_str);
				fprintf(stderr,"test2:result_str=%s\n",result_str);
				return 0;
			case 1:
				fprintf(stderr,"test2:Sysetm internal error (1).\n");
				break;
			case 2:
				fprintf(stderr,"test2:Sysetm internal error (2).\n");
				break;
			case 3:
				fprintf(stderr,"test2:Storage media is busy.\n");
				break;
			case 4:
				fprintf(stderr,"test2:Storage operation time out.\n");
				break;
			case 5:
				fprintf(stderr,"test2:No left space on storage media.\n");
				break;
			default:
				fprintf(stderr,"test2:Sysetm internal error (3).\n");
				break;
			}
	}
	else
	{
		return -1;
	}
	return 0;
}


int cgiMain()
{
//	fwRule *rule;
	struct list *lpublic;
	struct list *lcontrol;
//	int i=0;

		//�õ�rule�����ͣ� fileter dnat  snat
	char *encry=(char *)malloc(BUF_LEN);	
	char *cmd=(char*)malloc(128);	
	char *str;
	char lan[3]; 
	FILE *fp1;		
		
	char *file_name=(char *)malloc(128); //ԭ������ char �˴��� char *        
	memset(file_name,0,128);	

	lpublic=get_chain_head("../htdocs/text/public.txt");
	lcontrol=get_chain_head("../htdocs/text/control.txt");	
	
 	memset(encry,0,BUF_LEN);
	memset(cmd,0,128);
  	cgiFormStringNoNewlines("UN", encry, BUF_LEN);	
	cgiFormStringNoNewlines("Nb", file_name, 128);    //��ԭ�����ط��鿴 wp_del.cgi �д���  		
  	str=dcryption(encry);
  	if(str==NULL)
  	{
		ShowErrorPage(search(lpublic,"ill_user")); 		 /*�û��Ƿ�*/
		return 0;
  	}
	cgiHeaderContentType("text/html");
	
 	if((fp1=fopen("../htdocs/text/public.txt","r"))==NULL)		   /*��ֻ����ʽ����Դ�ļ�*/
 	{
		ShowAlert(search(lpublic,"error_open"));
		return 0;
	}
	fseek(fp1,4,0);						  /*���ļ�ָ���Ƶ����ļ���4���ֽڴ�����lan=֮��*/
	fgets(lan,3,fp1); 	 
	fclose(fp1);
		  	

	


//	else if(  )
	fprintf( cgiOut, "<html xmlns=\"http://www.w3.org/1999/xhtml\"> \n" );
	fprintf( cgiOut, "<head> \n" );
	fprintf( cgiOut, "<meta http-equiv=Content-Type content=text/html; charset=gb2312> \n" );
	//�������仰���ڽ�ֹҳ�滺��
  	fprintf( cgiOut, "<META   HTTP-EQUIV=\"pragma\"   CONTENT=\"no-cache\"> \n");
  	fprintf( cgiOut, "<META   HTTP-EQUIV=\"Cache-Control\"   CONTENT=\"no-cache,   must-revalidate\"> \n" );
  	fprintf( cgiOut, "<META   HTTP-EQUIV=\"expires\"   CONTENT=\"Wed,   26   Feb   1997   08:21:57   GMT\">	\n");
  	
	fprintf( cgiOut, "<title>%s</title> \n", search( lcontrol, "del" ) );
	fprintf( cgiOut, "<link rel=stylesheet href=/style.css type=text/css> \n" );
	fprintf( cgiOut, "<style type=text/css> \n" );
	fprintf( cgiOut, ".usrlis {overflow-x:hidden; overflow:auto; width: 416px; height: 270px; clip: rect( ); padding-top: 0px; padding-right: 0px; padding-bottom: 0px; padding-left: 0px} \n" );
	fprintf( cgiOut, "</style> \n" );
	fprintf( cgiOut, "<style type=text/css> \n" );
	fprintf( cgiOut, "tr.even td { \n" );
	fprintf( cgiOut, "background-color: #eee; \n" );
	fprintf( cgiOut, "} \n" );
	fprintf( cgiOut, "tr.odd td { \n" );
	fprintf( cgiOut, "background-color: #fff; \n" );
	fprintf( cgiOut, "} \n" );
	fprintf( cgiOut, "tr.changed td { \n" );
	fprintf( cgiOut, "background-color: #ffd; \n" );
	fprintf( cgiOut, "} \n" );
	fprintf( cgiOut, " \n" ); 
	fprintf( cgiOut, "tr.new td { \n" );  
	fprintf( cgiOut, "background-color: #dfd; \n" );
	fprintf( cgiOut, "} \n" );
	fprintf( cgiOut, "</style> \n" );
	fprintf( cgiOut, "</head> \n" );		
	fprintf( cgiOut, "<body> \n" );
	
{
	
	{
		
		//sprintf(cmd,"/mnt/shell/boot.sh  %s >/dev/null 2>$1",file_name);
		char result_str[64];
		char cmdstr[128] = {0};
		int ret,lenth=0;
		FILE *fp = 	NULL;
		char *ptr=NULL;
		
		sprintf(cmdstr,"sor.sh imgmd5 %s 120",file_name);
		fp = popen( cmdstr, "r" );
		if(fp)
		{
			fgets( result_str, 128, fp );
			ret = pclose(fp);
			
			switch (WEXITSTATUS(ret)) {
				case 0:
					
					ptr=result_str;
					ptr+=strlen(result_str);
					*(ptr-1)='\0';
					ShowAlert(result_str);
					break;
				case 1:
					ShowAlert("Sysetm internal error (1)");
					break;
				case 2:
					ShowAlert("Sysetm internal error (2)");
					break;
				case 3:
					ShowAlert("Storage media is busy");
					break;
				case 4:
					ShowAlert("Storage operation time out");
					break;
				case 5:
					ShowAlert("No left space on storage media");
					break;
				default:
					ShowAlert("Sysetm internal error (3)");
					break;
				}
		}
		else
		{
			ShowAlert(search(lpublic,"oper_fail"));
		}
			
		

		fprintf( cgiOut, "<script type='text/javascript'>\n" );
   		fprintf( cgiOut, "window.location.href='wp_version_del.cgi?UN=%s';\n", encry);
		fprintf( cgiOut, "</script>\n" );		
		fprintf( cgiOut, "</body>\n" );
		fprintf( cgiOut, "</html>\n" );
	}

}

	
	free(encry);
	release(lpublic); 
	
	return 0;	
}

