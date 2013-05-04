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
* wp_bk_import.c
*
*
* CREATOR:
* autelan.software.Network Dep. team
* zhouym@autelan.com
*
* DESCRIPTION:
* system function for import file
*
*
*******************************************************************************/
#include <stdio.h>
#include "cgic.h"
#include <string.h>
#include <fcntl.h> 
#include <sys/stat.h> 
#include <stdlib.h>
#include "ws_err.h"
#include "ws_usrinfo.h"
#include "ws_ec.h"
#include <sys/wait.h>

int ShowImportPage();     /*m������ܺ���ַ���*/
int getFileName(char *fpath);     /*����ϴ�ͼƬ������fname��ȫ��fpath*/
int upfile(char *fpath);           /*�ϴ��ļ�ȫ��Ϊfpath�������ļ�*/


int cgiMain()
{	
	ShowImportPage();  	
	return 0;
}

int ShowImportPage()
{ 
  
  char *encry=(char *)malloc(BUF_LEN);      		

  char imp_encry[BUF_LEN];  
  if(cgiFormSubmitClicked("upload_file") != cgiFormSuccess)
  {
    memset(encry,0,BUF_LEN);
    cgiFormStringNoNewlines("UN", encry, BUF_LEN); 
    
    if(strcmp(encry,"123456")!=0)
    {
      ShowErrorPage("�û��Ƿ�");          /*�û��Ƿ�*/
      return 0;
    }
    memset(imp_encry,0,BUF_LEN);                   /*�����ʱ����*/
  }
  cgiFormStringNoNewlines("encry_import",imp_encry,BUF_LEN);
  

  /***********************2008.5.26*********************/
  cgiHeaderContentType("text/html");

  fprintf(cgiOut,"<html xmlns=\"http://www.w3.org/1999/xhtml\"><head>");
  fprintf(cgiOut,"<meta http-equiv=Content-Type content=text/html; charset=gb2312>");
  fprintf(cgiOut,"<title>%s</title>","���������ļ�");  
  fprintf(cgiOut,"<body>");
  
  if(cgiFormSubmitClicked("upload_file") == cgiFormSuccess)
  {
	 
    char *fpath = (char *)malloc(PATH_LENG);
    int ret_fpath;
    memset(fpath,0,PATH_LENG);
    ret_fpath=getFileName(fpath);
	
    if(ret_fpath==1)
    {
  	int ret_upfile;
  	ret_upfile=upfile(fpath);

  	if(ret_upfile==1)
  	  ShowAlert("�ϴ��ɹ�");
  	else if(ret_upfile==0)
  	  ShowAlert("�ϴ�ʧ��");
    }
    else
    {
  	  ShowAlert("���ļ��ϴ�");
    }
    
  } 
   fprintf(cgiOut,"<form method=post encType=multipart/form-data>");
   fprintf(cgiOut,"<table>"\
    "<tr>");
	
	fprintf(cgiOut,"<td>"\
    "<table width=480 border=0 cellspacing=0 cellpadding=0>"\
	"<tr>"\
	"<td align=left>"\
	"<p>%s<input type=\"file\" size=\"30\" name=\"file\" value=\"\"></p>","��������");
	fprintf(cgiOut,"<td><input type=submit name=upload_file value=\"%s\"></td>","�ϴ�");
    fprintf(cgiOut,"</tr>");

				
              if(cgiFormSubmitClicked("upload_file") != cgiFormSuccess)
			   {
				 fprintf(cgiOut,"<tr><td colspan=3><input type=hidden name=encry_import value=%s></td></tr>",encry);
			   }
			   else if(cgiFormSubmitClicked("upload_file") == cgiFormSuccess)
			   {
				 fprintf(cgiOut,"<tr><td colspan=3><input type=hidden name=encry_import value=%s></td></tr>",imp_encry);
			   }	
			   
fprintf(cgiOut,"</table>");
fprintf(cgiOut,"</form>"\
        "</body>"\
        "</html>");
free(encry);
return 0;
}          



int getFileName(char *fpath)        /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
{
  
  cgiFilePtr file;
  char name[1024];    /*��ű���·����*/ 
  char *tmpStr=NULL;   
  int t;
  
  if (cgiFormFileName("file", name, sizeof(name)) != cgiFormSuccess) 
    return 0;


  if (cgiFormFileOpen("file", &file) != cgiFormSuccess) 
  {
	fprintf(stderr, "Could not open the file.<p>\n");
    return 0;
  }

  t=-1; 
  //��·�����������û��ļ��� 
  while(1){ 
  tmpStr=strstr(name+t+1,"\\");   /*��name+t+1��Ѱ��"\\"*/
  if(NULL==tmpStr)                /*�ɹ�����λ�ã����򷵻�NULL*/
  tmpStr=strstr(name+t+1,"/");    /*  if "\\" is not path separator, try "/"   */
  if(NULL!=tmpStr) 
    t=(int)(tmpStr-name);         /*����ҵ���t�洢ƫ�Ƶ�ַ*/       
  else 
    break;  
  } 
  strcpy(fpath,name+t+1);     /*���ļ�ȫ������fpath*/ 
  
  return 1;
}


int upfile(char *fpath)        /*�ɹ� ����1�����򷵻�0������-1��ʾû���ļ��ϴ�*/
{
    
	char *path_conf=(char *)malloc(1024);
	memset(path_conf,0,1024);

	int status=0,ret=-1;
	
	strcpy(path_conf,"/mnt/conf_xml.conf");
	
  cgiFilePtr file;
  char name[1024];    /*��ű���·����*/ 
  int targetFile; 
  mode_t mode;

  char *tempf=(char *)malloc(128);

  char buffer[1024]; 
  int got; 
  if (cgiFormFileName("file", name, sizeof(name)) != cgiFormSuccess) 
  {
    free(path_conf);
	free(tempf);
    return -1; 
  }
  
  if (cgiFormFileOpen("file", &file) != cgiFormSuccess) 
  {
    free(path_conf);
	free(tempf);

    return 0;
  }  

  mode=S_IRWXU|S_IRGRP|S_IROTH; 
  //�ڵ�ǰĿ¼�½����µ��ļ�����һ������ʵ������·�������˴��ĺ�������cgi�������ڵ�Ŀ¼����ǰĿ¼�����������ļ� 
  targetFile=open(path_conf,O_RDWR|O_CREAT|O_TRUNC|O_APPEND,mode); 
  if(targetFile == -1)
  { 
    free(path_conf);
	free(tempf);

    return 0; 
  } 
  //��ϵͳ��ʱ�ļ��ж����ļ����ݣ����ŵ��մ�����Ŀ���ļ��� 
  while (cgiFormFileRead(file, buffer, 1024, &got) ==cgiFormSuccess)
  { 
    if(got>0) 
    write(targetFile,buffer,got);   /*�������ļ�������д����������ļ�*/
  } 
  
  cgiFormFileClose(file); 
  close(targetFile);  

  memset(tempf,0,128);
  sprintf(tempf,"sudo sor.sh cp conf_xml.conf %d > /dev/null",SHORT_SORT);
  status = system(tempf);
  ret = WEXITSTATUS(status);

  free(path_conf);
  free(tempf);

  if(ret==0)
  	return 1;
  else
  	return -1;
  
}


