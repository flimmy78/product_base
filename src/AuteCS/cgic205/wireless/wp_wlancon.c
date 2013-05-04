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
* wp_wlancon.c
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


#include <stdio.h>
#include "cgic.h"
#include <string.h>
#include <stdlib.h>
#include "ws_usrinfo.h"
#include "ws_err.h"
#include "ws_ec.h"
#include "wcpss/asd/asd.h"
#include "wcpss/wid/WID.h"
#include "dbus/wcpss/dcli_wid_wtp.h"
#include "dbus/wcpss/dcli_wid_wlan.h"
#include "dbus/wcpss/ACDbusDef1.h"
#include "ws_dcli_wlans.h"
#include "ws_security.h"
#include "ws_sta.h"
#include "ws_log_conf.h"
#include <sys/wait.h>
#include "ws_dbus_list_interface.h"


int ShowWlanconPage(char *m,char *n,char *t,char *pn,char *ins_id,instance_parameter *ins_para,struct list *lpublic,struct list *lwlan);  /*m��������ַ�����n����wlan id��t����wlan name ��r����security id��intf����interafce��Hessid����Hideessid��s����service state*/
void ConWlan(instance_parameter *ins_para,int id,int inter_num,int security_flag,struct list *lpublic,struct list *lwlan);

int cgiMain()
{  
  char encry[BUF_LEN] = { 0 };              
  char *str = NULL;                
  char ID[5] = { 0 };
  char wlan_name[20] = { 0 };    
  char inter[20] = { 0 };
  char pno[10] = { 0 }; 
  char instance_id[10] = { 0 };
  struct list *lpublic = NULL;   /*����public.txt�ļ�������ͷ*/
  struct list *lwlan = NULL;     /*����wlan.txt�ļ�������ͷ*/  
  instance_parameter *paraHead1 = NULL;
  dbus_parameter ins_para;

  lpublic=get_chain_head("../htdocs/text/public.txt");
  lwlan=get_chain_head("../htdocs/text/wlan.txt");
  
  DcliWInit();
  ccgi_dbus_init();
  memset(encry,0,sizeof(encry));
  memset(ID,0,sizeof(ID));
  memset(wlan_name,0,sizeof(wlan_name));
  memset(inter,0,sizeof(inter));
  memset(pno,0,sizeof(pno)); 
  memset(instance_id,0,sizeof(instance_id));
  if(cgiFormStringNoNewlines("UN", encry, BUF_LEN)!=cgiFormNotFound )  /*�״ν����ҳ*/
  {
	cgiFormStringNoNewlines("ID", ID, 5);	
	cgiFormStringNoNewlines("Na", wlan_name, 20);  
	cgiFormStringNoNewlines("PN",pno,10);
	cgiFormStringNoNewlines("INSTANCE_ID", instance_id, 10);
  }  
  else
  {  
    cgiFormStringNoNewlines("encry_conwlanser",encry,BUF_LEN);
    cgiFormStringNoNewlines("wlan_id",ID,5);  
    cgiFormStringNoNewlines("wlan_name",wlan_name,20);		
	cgiFormStringNoNewlines("bind_interface",inter,20);
	cgiFormStringNoNewlines("page_no",pno,10);
	cgiFormStringNoNewlines("instance_id",instance_id,10);  
  }  
  
  if(strcmp(instance_id,"")==0)
  {	
	list_instance_parameter(&paraHead1, INSTANCE_STATE_WEB);	
	if(paraHead1)
	{
		snprintf(instance_id,sizeof(instance_id)-1,"%d-%d-%d",paraHead1->parameter.slot_id,paraHead1->parameter.local_id,paraHead1->parameter.instance_id); 
	} 
  }
  else
  {
	get_slotID_localID_instanceID(instance_id,&ins_para);	
	get_instance_dbus_connection(ins_para, &paraHead1, INSTANCE_STATE_WEB);
  }
  
  str=dcryption(encry);
  if(str==NULL)
	ShowErrorPage(search(lpublic,"ill_user"));			  /*�û��Ƿ�*/
  else
	ShowWlanconPage(encry,ID,wlan_name,pno,instance_id,paraHead1,lpublic,lwlan);
  
  release(lpublic);  
  release(lwlan);
  free_instance_parameter_list(&paraHead1);
  destroy_ccgi_dbus();
  return 0;
}

int ShowWlanconPage(char *m,char *n,char *t,char *pn,char *ins_id,instance_parameter *ins_para,struct list *lpublic,struct list *lwlan)
{    
  char IsSubmit[5] = { 0 };
  FILE *fp = NULL;
  char BindInter[20] = { 0 };
  char *endptr = NULL;  
  char *retu = NULL;
  int limit = 0,wlan_id = 0,i = 0,status = 1;  
  int result1 = 0,result2 = 0;
  struct dcli_security *head = NULL,*q = NULL;          /*���security��Ϣ������ͷ*/   
  DCLI_WLAN_API_GROUP *whead = NULL;
  struct ifi *inf = NULL;
  int inter_num = 0;             /*���interface�ĸ���*/
  int security_flag = 0;		   /*security_flag==0��ʾWLANû�а󶨰�ȫ����*/
  int sec_num = 0;             /*���security�ĸ���*/
  wlan_id=strtoul(n,&endptr,10);	/*charת��int��10����ʮ����*/	
  int ap_reboot_result = 0,remove_nasid_result = 0;
  char alt[100] = { 0 };
  char max_wlan_num[10] = { 0 };
  int result = cgiFormNotFound,flag = 1,bind_result = 1;
  char **responses; 
  cgiHeaderContentType("text/html");
  fprintf(cgiOut,"<html xmlns=\"http://www.w3.org/1999/xhtml\"><head>");
  fprintf(cgiOut,"<meta http-equiv=Content-Type content=text/html; charset=gb2312>");
  fprintf(cgiOut,"<title>Wlan</title>");
  fprintf(cgiOut,"<link rel=stylesheet href=/style.css type=text/css>"\
  	"<style type=text/css>"\
  	".a3{width:30;border:0; text-align:center}"\
  	"</style>"\
  "</head>"\
  "<script src=/ip.js>"\
  "</script>"\
  "<script type=\"text/javascript\">"\
	"function checkAll(e, itemName)"\
	"{"\
	  "var aa = document.getElementsByName(itemName);"\
	  "for (var x=0; x<aa.length; x++)"\
	  "aa[x].checked = e.checked;"\
	"}"\
	"function uncheckAll(e, itemName)"\
	"{"\
	  "var aa = document.getElementsByName(itemName);"\
	  "if(!(e.checked))"\
	  "{"\
	  	"if(aa[7].checked)"\
  		"{"\
		  "aa[7].checked = false;"\
  		"}"\
	  "}"\
	"}"\
	"function isTime(str)"\
		"{"\
			"if(str!=null&&str!=\"\")"\
			"{"\
				"var a = str.match(/^(\\d{1,2})(:)?(\\d{1,2})\\2(\\d{1,2})$/);"\
				"if (a == null)"\
				"{"\
					"alert(\"%s\");",search(lpublic,"timewrong"));
					fprintf(cgiOut,"document.all.retime.value = \"\";"\
					"document.all.retime.focus();"\
				"}"\
				"if (a[1]>24 || a[3]>60 || a[4]>60)"\
				"{"\
					"alert(\"%s\");"\
					"document.all.retime.value = \"\";"\
					"document.all.retime.focus();"\
				"}"\
				"return true;"\
			"}"\
	"}",search(lpublic,"timewrong"));
  fprintf(cgiOut,"</script>"\
  "<body>");	  
  memset(IsSubmit,0,sizeof(IsSubmit));  
  cgiFormStringNoNewlines("SubmitFlag", IsSubmit, 5);  
  if(ins_para)
  {
	  result2=show_security_list(ins_para->parameter,ins_para->connection,&head,&sec_num);
  } 
  if((cgiFormSubmitClicked("wlanconser_apply") == cgiFormSuccess)&&(strcmp(IsSubmit,"")))
  {  	
  	if(ins_para)
	{
		result1=show_wlan_one(ins_para->parameter,ins_para->connection,wlan_id,&whead); 
	} 
	if(result1 == 1)
	{
		if((whead != NULL)&&(whead->WLAN[0] != NULL))
		{
			for(inf = whead->WLAN[0]->Wlan_Ifi; (NULL != inf); inf = inf->ifi_next)
			{
				inter_num++;	
			}
		}
	}
	
    if((result1 == 1)&&(whead)&&(whead->WLAN[0])&&(whead->WLAN[0]->SecurityID!=0))
      security_flag=1;

	if(ins_para)
	{
		ConWlan(ins_para,wlan_id,inter_num,security_flag,lpublic,lwlan);
	} 

	if(result1==1)
	{
	  Free_one_wlan_head(whead);
	}
  }
  if((cgiFormSubmitClicked("ap_reboot") == cgiFormSuccess)&&(strcmp(IsSubmit,"")))
  {
  	if(ins_para)
	{
		ap_reboot_result=set_ap_reboot_by_wlanid_func(ins_para->parameter,ins_para->connection,n);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
																								 /*����-2��ʾwlan id should be 1 to WLAN_NUM-1*/
																								 /*����-3��ʾwlan id does not exist������-4��ʾerror*/
	} 
	switch(ap_reboot_result)
	{
		case SNMPD_CONNECTION_ERROR:
    	case 0:ShowAlert(search(lpublic,"ap_reboot_fail"));
			   break;
		case 1:ShowAlert(search(lpublic,"ap_reboot_succ"));
			   break;
		case -1:ShowAlert(search(lpublic,"unknown_id_format"));
			    break;
		case -2:{
		  		  memset(alt,0,sizeof(alt));
				  strncpy(alt,search(lwlan,"wlan_id_illegal1"),sizeof(alt)-1);
				  memset(max_wlan_num,0,sizeof(max_wlan_num));
				  snprintf(max_wlan_num,sizeof(max_wlan_num)-1,"%d",WLAN_NUM-1);
				  strncat(alt,max_wlan_num,sizeof(alt)-strlen(alt)-1);
				  strncat(alt,search(lwlan,"wlan_id_illegal2"),sizeof(alt)-strlen(alt)-1);
		  	      ShowAlert(alt);
			  	  break;
	      	    }
		case -3:ShowAlert(search(lwlan,"wlan_not_exist"));
				break;
		case -4:ShowAlert(search(lpublic,"error"));
				break;
    }
  }
  if((cgiFormSubmitClicked("remove_nasid") == cgiFormSuccess)&&(strcmp(IsSubmit,"")))
  {
    result = cgiFormStringMultiple("bind_interface", &responses);
	if(result != cgiFormNotFound)
	{
		i = 0;	
		bind_result=1;
		while((responses[i])&&(bind_result==1))
		{
			if(ins_para)
			{
				remove_nasid_result=remove_interface_nasid_cmd_func(ins_para->parameter,ins_para->connection,wlan_id,responses[i]); /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																																   /*����-1��ʾthe length of if_name is excel the limit of 16*/
																																   /*����-2��ʾwlan id is not exist*/
																																   /*����-3��ʾwlan WlanID is not binding interface if_name*/
																																   /*����-4��ʾinterface error*/
																																   /*����-5��ʾno nas_id needed,please use <wlan apply interface IFNAME>,without nas_identifier*/
																																   /*����-6��ʾwlan be enable,please service disable first*/
																																   /*����-7��ʾerror*/
			} 
			switch(remove_nasid_result)
			{
				case SNMPD_CONNECTION_ERROR:
				case 0:ShowAlert(search(lwlan,"remove_nasid_fail"));
					   bind_result=0;
					   break;
				case 1:break;
				case -1:ShowAlert(search(lpublic,"interface_too_long"));
				        bind_result=0;
						break;
				case -2:ShowAlert(search(lwlan,"wlan_not_exist"));
						bind_result=0;
						break;
				case -3:{
						  memset(alt,0,sizeof(alt));
						  strncpy(alt,search(lwlan,"wlan_dont_bind_inter1"),sizeof(alt)-1);
						  strncat(alt,n,sizeof(alt)-strlen(alt)-1);
						  strncat(alt,search(lwlan,"wlan_dont_bind_inter2"),sizeof(alt)-strlen(alt)-1);
						  strncat(alt,responses[i],sizeof(alt)-strlen(alt)-1);
						  strncat(alt,search(lwlan,"wtp_over_wep_wlan_count2"),sizeof(alt)-strlen(alt)-1);
						  ShowAlert(alt);
						  bind_result=0;
						  break;
						}
				case -4:{
						  memset(alt,0,sizeof(alt));
						  strncpy(alt,search(lwlan,"interface"),sizeof(alt)-1);
						  strncat(alt,search(lpublic,"inter_error"),sizeof(alt)-strlen(alt)-1);
						  ShowAlert(alt);
						  bind_result=0;
						  break;
						}
				case -5:ShowAlert(search(lwlan,"no_nasid_to_remove"));
						bind_result=0;
						break;
				case -6:ShowAlert(search(lwlan,"dis_wlan"));
						bind_result=0;
						break;
				case -7:ShowAlert(search(lpublic,"error"));
				        bind_result=0;
						break;
			}
			i++;
		}
		cgiStringArrayFree(responses);
			
		if(bind_result==0)
		  flag=0;
	}
	else
	{
	    ShowAlert(search(lwlan,"select_inter"));	
	    flag=0;
	}
	if(flag==1)
	  ShowAlert(search(lwlan,"remove_nasid_succ"));				       
  }
  fprintf(cgiOut,"<form method=post>"\
  "<div align=center>"\
  "<table width=976 border=0 cellpadding=0 cellspacing=0>"\
  "<tr>"\
    "<td width=8 align=left valign=top background=/images/di22.jpg><img src=/images/youce4.jpg width=8 height=30/></td>"\
    "<td width=51 align=left valign=bottom background=/images/di22.jpg><img src=/images/youce33.jpg width=37 height=24/></td>"\
    "<td width=153 align=left valign=bottom id=titleen background=/images/di22.jpg>WLAN</td>"\
    "<td width=690 align=right valign=bottom background=/images/di22.jpg>");	
	   
    	  fprintf(cgiOut,"<table width=155 border=0 cellspacing=0 cellpadding=0>"\
          "<tr>");
		  if(sec_num>0)      /*���security����*/
            fprintf(cgiOut,"<td width=62 align=center><input id=but type=submit name=wlanconser_apply style=background-image:url(/images/%s) value=""></td>",search(lpublic,"img_ok"));		  
		  else
		  	fprintf(cgiOut,"<td width=62 align=center><a href=wp_wlanlis.cgi?UN=%s&PN=%s target=mainFrame><img src=/images/%s border=0 width=62 height=20/></a></td>",m,pn,search(lpublic,"img_ok"));
          fprintf(cgiOut,"<td width=62 align=center><a href=wp_wlanlis.cgi?UN=%s&PN=%s&INSTANCE_ID=%s target=mainFrame><img src=/images/%s border=0 width=62 height=20/></a></td>",m,pn,ins_id,search(lpublic,"img_cancel"));
		  fprintf(cgiOut,"</tr>"\
          "</table>");

      fprintf(cgiOut,"</td>"\
    "<td width=74 align=right valign=top background=/images/di22.jpg><img src=/images/youce3.jpg width=31 height=30/></td>"\
  "</tr>"\
  "<tr>"\
    "<td colspan=5 align=center valign=middle><table width=976 border=0 cellpadding=0 cellspacing=0 bgcolor=#f0eff0>"\
      "<tr>"\
        "<td width=12 align=left valign=top background=/images/di888.jpg>&nbsp;</td>"\
        "<td width=948><table width=947 border=0 cellspacing=0 cellpadding=0>"\
            "<tr height=4 valign=bottom>"\
              "<td width=120>&nbsp;</td>"\
              "<td width=827 valign=bottom><img src=/images/bottom_05.gif width=827 height=4/></td>"\
            "</tr>"\
            "<tr>"\
              "<td><table width=120 border=0 cellspacing=0 cellpadding=0>"\
                   "<tr height=25>"\
                    "<td id=tdleft>&nbsp;</td>"\
                  "</tr>");	              
	              fprintf(cgiOut,"<tr height=26>"\
                    "<td align=left id=tdleft background=/images/bottom_bg.gif style=\"border-right:0\"><font id=%s>%s</font><font id=yingwen_san> WLAN</font></td>",search(lpublic,"menu_san"),search(lpublic,"config"));   /*ͻ����ʾ*/
                  fprintf(cgiOut,"</tr>"\
                  "<tr height=25>"\
					"<td align=left id=tdleft><a href=wp_wlannew.cgi?UN=%s target=mainFrame class=top><font id=%s>%s</font><font id=yingwen_san> WLAN</font></a></td>",m,search(lpublic,"menu_san"),search(lpublic,"create"));                       
                  fprintf(cgiOut,"</tr>");
					 fprintf(cgiOut,"<tr height=25>"\
					  "<td align=left id=tdleft><a href=wp_wlanbw.cgi?UN=%s target=mainFrame class=top><font id=yingwen_san>MAC</font><font id=%s> %s</font></a></td>",m,search(lpublic,"menu_san"),search(lwlan,"mac_filter"));                       
                    fprintf(cgiOut,"</tr>");
				  result1 = 0;
				  if(ins_para)
				  {
					  result1=show_wlan_one(ins_para->parameter,ins_para->connection,wlan_id,&whead); 
				  } 
				  inter_num = 0;
				  if(result1 == 1)
				  {
					if((whead != NULL)&&(whead->WLAN[0] != NULL))
					{
						for(inf = whead->WLAN[0]->Wlan_Ifi; (NULL != inf); inf = inf->ifi_next)
						{
							inter_num++;	
						}
					}
				  }
				  
				  limit=29;
				  if((inter_num>0)&&(whead)&&(whead->WLAN[0])&&(whead->WLAN[0]->Status==1))  /*�Ѱ󶨽ӿڣ���WLAN״̬Ϊdisable*/
				  	limit+=3;
                  for(i=0;i<limit;i++)
	              {
  				    fprintf(cgiOut,"<tr height=25>"\
                      "<td id=tdleft>&nbsp;</td>"\
                    "</tr>");
	              }
                fprintf(cgiOut,"</table>"\
              "</td>"\
              "<td align=left valign=top style=\"background-color:#ffffff; border-right:1px solid #707070; padding-left:30px; padding-top:10px\">"\
       "<table width=700 border=0 cellspacing=0 cellpadding=0>"\
		"<tr>"\
		  "<td id=ins_style>%s:%s</td>",search(lpublic,"instance"),ins_id);
		fprintf(cgiOut,"</tr>"\
		"<tr>"\
		  "<td><table width=700 border=0 cellspacing=0 cellpadding=0>");
	  status = system("bind_inter.sh"); 	  
	  if(result2 == 1)	/*��ʾ����security����Ϣ��head����security��Ϣ���������ͷ*/
		  {
            if(sec_num>0)      /*���security����*/
            {
                  fprintf(cgiOut,"<tr height=30 align=left>");
                    fprintf(cgiOut,"<td id=thead5 align=left>%s %s</td>",search(lpublic,"configure"),t);
                  fprintf(cgiOut,"</tr>"\
               "</table>"\
          "</td>"\
        "</tr>"\
		"<tr><td align=left style=\"padding-left:20px\">");
			  	fprintf(cgiOut,"<table width=700 border=0 cellspacing=0 cellpadding=0>");
                fprintf(cgiOut,"<tr height=30>");				  
			
				  	fprintf(cgiOut,"<td width=120>%s:</td>",search(lwlan,"bind_security"));
					if((result1 == 1)&&(whead)&&(whead->WLAN[0])&&(whead->WLAN[0]->Status!=1))   /*���WLAN״̬Ϊenable����security�������򲻿���*/
					  fprintf(cgiOut,"<td width=100 align=left><select name=sec_id id=sec_id style=width:100px disabled>");
					else
					  fprintf(cgiOut,"<td width=100 align=left><select name=sec_id id=sec_id style=width:100px>");					
				  if((result1 == 1)&&(whead)&&(whead->WLAN[0])&&(whead->WLAN[0]->SecurityID!=0))       /*WLAN�Ѿ���security*/
				    fprintf(cgiOut,"<option value=>");				  
				  for(i=0,q=head;((i<sec_num)&&(NULL != q));i++,q=q->next)
				  {     
					fprintf(cgiOut,"<option value=%d>security %d",q->SecurityID,q->SecurityID);
				  } 
	              fprintf(cgiOut,"</select></td>");
				  fprintf(cgiOut,"<td width=370 align=left><font color=red>(%s)</font>"\
				  										  "<input type=submit style=\"width:100px; margin-left:20px\" border=0 name=ap_reboot style=background-image:url(/images/SubBackGif.gif) value=\"%s\">"\
				  										  "</td>",search(lwlan,"con_bind"),search(lpublic,"ap_reboot"));
                fprintf(cgiOut,"</tr>"\
                "<tr height=30>"\
                 "<td>%s:</td>",search(lwlan,"bind_interface"));
                 fprintf(cgiOut,"<td align=left>");
				 if(status==0)
				 {
				   if((result1 == 1)&&(whead)&&(whead->WLAN[0])&&(whead->WLAN[0]->Status!=1))   /*���WLAN״̬Ϊenable���󶨽ӿڵ������򲻿���*/
				     fprintf(cgiOut,"<select name=bind_interface id=bind_interface multiple=multiple size=4 style=width:100px disabled>");
				   else
				     fprintf(cgiOut,"<select name=bind_interface  id=bind_interface multiple=multiple size=4 style=width:100px>");
				   if((fp=fopen("/var/run/apache2/bind_inter.tmp","r"))==NULL)		 /*��ֻ����ʽ����Դ�ļ�*/
				   {
					   ShowAlert(search(lpublic,"error_open"));
			       }
				   else
				   {
					   memset(BindInter,0,sizeof(BindInter));
					   retu=fgets(BindInter,20,fp);
					   while(retu!=NULL)
					   {
						 fprintf(cgiOut,"<option value=%s>%s",retu,retu);
						 memset(BindInter,0,sizeof(BindInter));
						 retu=fgets(BindInter,20,fp);
					   }				   
					   fclose(fp);
				   }
		           fprintf(cgiOut,"</select>");
				 }
				 else
				 {
  				    fprintf(cgiOut,"%s",search(lpublic,"exec_shell_fail"));
				 }				 
				fprintf(cgiOut,"</td>");
				  fprintf(cgiOut,"<td align=left><font color=red>(%s)</font></td>",search(lwlan,"con_int"));
                fprintf(cgiOut,"</tr>"\
                "<tr height=30>"\
                 "<td>WLAN %s:</td>",search(lwlan,"service"));
                 fprintf(cgiOut,"<td align=left>");
				    if((result1 == 1)&&(whead)&&(whead->WLAN[0])&&(whead->WLAN[0]->SecurityID!=0)&&(inter_num>0))  /*�Ѱ󶨰�ȫ���Ժͽӿ�*/
				 	  fprintf(cgiOut,"<select name=wlan_service id=wlan_service style=width:100px>");
					else
					  fprintf(cgiOut,"<select name=wlan_service id=wlan_service style=width:100px disabled>");
				 	fprintf(cgiOut,"<option value=>"\
  				    "<option value=disable>disable"\
  				    "<option value=enable>enable"\
		          "</select></td>"\
				  "<td align=left><font color=red>(%s)</font></td>",search(lwlan,"con_able"));
                fprintf(cgiOut,"</tr>"\
				"<tr height=30>"\
				  "<td>ESSID:</td>");
				  if((result1 == 1)&&(whead)&&(whead->WLAN[0])&&(whead->WLAN[0]->Status!=1))   /*���WLAN״̬Ϊenable���޸�ESSID���ı��򲻿���*/
				    fprintf(cgiOut,"<td><input name=new_essid size=15 maxLength=32 value=\"\" style=\"background-color:#cccccc\" disabled></td>");
				  else
				  	fprintf(cgiOut,"<td><input name=new_essid size=15 maxLength=32 value=\"\" onkeypress=\"return event.keyCode!=32\"></td>");
				  fprintf(cgiOut,"<td align=left><font color=red>(%s ESSID)</font></td>",search(lpublic,"log_mod"));
				fprintf(cgiOut,"</tr>"\
				"<tr height=30>"\
                 "<td>%s HideEssid:</td>",search(lpublic,"config"));
				 if((result1 == 1)&&(whead)&&(whead->WLAN[0])&&(whead->WLAN[0]->Status!=1))   /*���WLAN״̬Ϊenable���Ƿ�����ESSID�������򲻿���*/
			       fprintf(cgiOut,"<td align=left><select name=set_hessid id=set_hessid style=width:100px disabled>");
				 else
				   fprintf(cgiOut,"<td align=left><select name=set_hessid id=set_hessid style=width:100px>");
				    fprintf(cgiOut,"<option value=>"\
					"<option value=no>no"\
  				    "<option value=yes>yes"\
		          "</select></td>");
				  fprintf(cgiOut,"<td align=left><font color=red>(%s)</font></td>",search(lwlan,"con_hidden"));
				fprintf(cgiOut,"</tr>"\
				"<tr height=30>"\
					"<td>NASID:</td>"\
					"<td align=left><input name=nasid size=15 maxLength=128 value=\"\"></td>"\
  				    "<td align=left><font color=red>(%s)</font>"\
  				    				"<input type=submit style=\"width:100px; margin-left:50px\" border=0 name=remove_nasid style=background-image:url(/images/SubBackGif.gif) value=\"%sNASID\">"\
  				    			    "</td>",search(lwlan,"most_nasid"),search(lpublic,"remove"));
				fprintf(cgiOut,"</tr>"\
					"<tr height=30>"\
						"<td>%s:</td>",search(lwlan,"wlan_sta_max"));
				fprintf(cgiOut,"<td ><input name=max_num size=15 maxLength=5 onkeypress=\"return event.keyCode>=48&&event.keyCode<=57\" value=\"\"></td>"\
				 				"<td align=left><font color=red>(1--65536)</font></td>"\
				"</tr>");
				fprintf(cgiOut,"<tr height=30>"\
						"<td>%s:</td>",search(lwlan,"balance_wlan"));
				fprintf(cgiOut,"<td colspan=2><select name=wlan_balance_type id=wlan_balance_type style=width:100px>"\
									"<option value=>"\
					                "<option value=number>number"\
  				    				"<option value=flow>flow"\
  				    				"<option value=disable>disable"\
					           "</select></td>"\
				"</tr>");
				fprintf(cgiOut,"<tr height=30>"\
						"<td>%s:</td>",search(lwlan,"wtp_triger_num"));
				fprintf(cgiOut,"<td ><input name=number_balance_value size=15 maxLength=2 onkeypress=\"return event.keyCode>=48&&event.keyCode<=57\" value=\"\"></td>"\
				 				"<td align=left><font color=red>(1--10)</font></td>"\
				"</tr>");
				fprintf(cgiOut,"<tr height=30>"\
						"<td>%s:</td>",search(lwlan,"wtp_flow_triger"));
				fprintf(cgiOut,"<td ><input name=flow_balance_value size=15 maxLength=2 onkeypress=\"return event.keyCode>=48&&event.keyCode<=57\" value=\"\"></td>"\
				 				"<td align=left><font color=red>(1--30)</font></td>"\
				"</tr>");
				if((result1 == 1)&&(inter_num>0)&&(whead)&&(whead->WLAN[0])&&(whead->WLAN[0]->Status==1))  /*�Ѱ󶨽ӿڣ���WLAN״̬Ϊdisable*/
				{
				  fprintf(cgiOut,"<tr style=\"padding-top:3px\">"\
						"<td>%s:</td>",search(lwlan,"delet_bind_inter"));
				  fprintf(cgiOut,"<td>"\
					             "<select name=del_bind_inter id=del_bind_inter multiple=multiple size=4 style=width:100px>");
				  				 if((whead)&&(whead->WLAN[0]))
				  				 {
									 for(inf = whead->WLAN[0]->Wlan_Ifi; (NULL != inf); inf = inf->ifi_next)
									 {
										fprintf(cgiOut,"<option value=%s>%s",inf->ifi_name,inf->ifi_name);
										inter_num++;	
									 } 
				  				 }
					             fprintf(cgiOut,"</select></td>"\
				 			     "<td align=left><font color=red>(%s)</font></td>",search(lwlan,"con_unbind"));
    			  fprintf(cgiOut,"</tr>");
				}
				fprintf(cgiOut,"<tr height=30>"\
						"<td>%s:</td>",search(lwlan,"con_roam_switch"));
				fprintf(cgiOut,"<td>");
								if((result1 == 1)&&(whead->WLAN[0]->Status!=1))   /*���WLAN״̬Ϊenable�����ο������͵������򲻿���*/
								  fprintf(cgiOut,"<select name=roam_type id=roam_type style=width:100px disabled>");
								else
								  fprintf(cgiOut,"<select name=roam_type id=roam_type style=width:100px>");
									fprintf(cgiOut,"<option value=>"\
  				    				/*"<option value=fast>fast"\*/
									"<option value=l3>l3"\
				 				"</select></td>"\
				 				"<td>");
								  if((result1 == 1)&&(whead)&&(whead->WLAN[0])&&(whead->WLAN[0]->Status!=1))   /*���WLAN״̬Ϊenable�����ο��ص������򲻿���*/
								  fprintf(cgiOut,"<select name=roam_state id=roam_state style=width:100px disabled>");
								else
								  fprintf(cgiOut,"<select name=roam_state id=roam_state style=width:100px>");
									fprintf(cgiOut,"<option value=>"\
  				    				"<option value=disable>disable"\
									"<option value=enable>enable"\
								"</select></td>");
				fprintf(cgiOut,"</tr>"\
				"<tr height=30>"\
						"<td>WLAN%s:</td>",search(lpublic,"uplink_traffic_limit_threshold"));
				fprintf(cgiOut,"<td><input name=wlan_uplink_traffic_limit_threshold size=15 maxLength=6 onkeypress=\"return event.keyCode>=48&&event.keyCode<=57\" value=\"\"></td>"\
						"<td align=left><font color=red>(0--300000)</font></td>"\
				"</tr>"\
				"<tr height=30>"\
						"<td>WLAN%s:</td>",search(lpublic,"downlink_traffic_limit_threshold"));
				fprintf(cgiOut,"<td><input name=wlan_downlink_traffic_limit_threshold size=15 maxLength=6 onkeypress=\"return event.keyCode>=48&&event.keyCode<=57\" value=\"\"></td>"\
						"<td align=left><font color=red>(0--300000)</font></td>"\
				"</tr>"\
				"<tr height=30>"\
					"<td>WDS %s:</td>",search(lwlan,"service"));
					fprintf(cgiOut,"<td>");
						if((result1 == 1)&&(whead)&&(whead->WLAN[0])&&(whead->WLAN[0]->Status==1))/*���WLAN״̬Ϊdisable������WDS״̬�������򲻿���*/
				    	  fprintf(cgiOut,"<select name=wds_state id=wds_state style=width:100px disabled>");
						else
						  fprintf(cgiOut,"<select name=wds_state id=wds_state style=width:100px>");
							fprintf(cgiOut,"<option value=>"\
			    				"<option value=disable>disable"\
							"<option value=enable>enable"\
		 				"</select></td>"\
					"<td align=left><font color=red>(%s)</font></td>",search(lwlan,"con_wds_service"));
				fprintf(cgiOut,"</tr>"\
				"<tr height=30>"\
						"<td>%s:</td>",search(lwlan,"uplink_detect"));
				fprintf(cgiOut,"<td>"\
								 "<div style=\"border-width:1;border-color:#a5acb2;border-style:solid;width:140;font-size:9pt\">"\
								 "<input type=text name='ip1' maxlength=3 class=a3 onKeyUp=\"mask(this,%s)\" onbeforepaste=mask_c()>.",search(lpublic,"ip_error"));
								 fprintf(cgiOut,"<input type=text name='ip2' maxlength=3 class=a3 onKeyUp=\"mask(this,%s)\" onbeforepaste=mask_c()>.",search(lpublic,"ip_error"));
								 fprintf(cgiOut,"<input type=text name='ip3' maxlength=3 class=a3 onKeyUp=\"mask(this,%s)\" onbeforepaste=mask_c()>.",search(lpublic,"ip_error"));
								 fprintf(cgiOut,"<input type=text name='ip4' maxlength=3 class=a3 onKeyUp=\"mask(this,%s)\" onbeforepaste=mask_c()>",search(lpublic,"ip_error"));
								 fprintf(cgiOut,"</div>"\
					           "</td>"\
				 				"<td><select name=uplink_detect_state id=uplink_detect_state style=width:100px>"\
				 					"<option value=>"\
  				    				"<option value=disable>disable"\
									"<option value=enable>enable"\
				 				"</select></td>"\
				"</tr>"\
				"<tr height=30>"\
						"<td>%s:</td>",search(lwlan,"multi_user_switch"));
				fprintf(cgiOut,"<td colspan=2><select name=multi_user_switch id=multi_user_switch style=width:100px>"\
									"<option value=>"\
					                "<option value=enable>enable"\
  				    				"<option value=disable>disable"\
					           "</select></td>"\
				"</tr>"\
				"<tr>"\
                    "<td id=sec1 colspan=3 style=padding-top:15px style=\"border-bottom:2px solid #53868b\">WLAN%s</td>",search(lpublic,"service_timer"));
                fprintf(cgiOut,"</tr>"\
				"<tr height=30 style=padding-top:10px>"\
					"<td>%s:</td>",search(lpublic,"switch"));
					fprintf(cgiOut,"<td>"\
						  "<select name=timer_type id=timer_type style=width:100px>"\
							"<option value=>"\
							"<option value=starttimer>starttimer"\
							"<option value=stoptimer>stoptimer"\
					"</select></td>"\
					"<td>"\
						  "<select name=timer_state id=timer_state style=width:100px>"\
							"<option value=>"\
							"<option value=disable>disable"\
							"<option value=enable>enable"\
					"</select></td>"\
				"</tr>"\
				"<tr height=30 style=padding-top:10px>"\
					"<td>%s:</td>",search(lpublic,"start_time"));
					fprintf(cgiOut,"<td><input name=start_time size=15 maxLength=8 onkeypress=\"return event.keyCode>=48&&event.keyCode<=58\" value=\"\" onblur=\"isTime(this.value);\"><font color=red>(HH:MM:SS)</font></td>"\
					"<td>"\
					  "<select name=start_timer_type id=start_timer_type style=width:100px>"\
						"<option value=>"\
						"<option value=once>once"\
						"<option value=cycle>cycle"\
					  "</select></td>"\
				"</tr>"\
				"<tr height=30>"\
					"<td>&nbsp;</td>"\
					"<td colspan=2>"\
					  "<input type=\"checkbox\" name=\"start_weekdays\" value=mon onclick=\"uncheckAll(this,'start_weekdays')\">%s",search(lpublic,"week_mon"));
					  fprintf(cgiOut,"<input type=\"checkbox\" style=margin-left:15px name=\"start_weekdays\" value=tue onclick=\"uncheckAll(this,'start_weekdays')\">%s",search(lpublic,"week_tue"));
					  fprintf(cgiOut,"<input type=\"checkbox\" style=margin-left:15px name=\"start_weekdays\" value=wed onclick=\"uncheckAll(this,'start_weekdays')\">%s",search(lpublic,"week_wed"));
					  fprintf(cgiOut,"<input type=\"checkbox\" style=margin-left:15px name=\"start_weekdays\" value=thu onclick=\"uncheckAll(this,'start_weekdays')\">%s",search(lpublic,"week_thu"));
					fprintf(cgiOut,"</td>"\
				"</tr>"\
				"<tr height=30>"\
					"<td>&nbsp;</td>"\
					"<td colspan=2>"\
					  "<input type=\"checkbox\" name=\"start_weekdays\" value=fri onclick=\"uncheckAll(this,'start_weekdays')\">%s",search(lpublic,"week_fri"));
					  fprintf(cgiOut,"<input type=\"checkbox\" style=margin-left:15px name=\"start_weekdays\" value=sat onclick=\"uncheckAll(this,'start_weekdays')\">%s",search(lpublic,"week_sat"));
					  fprintf(cgiOut,"<input type=\"checkbox\" style=margin-left:15px name=\"start_weekdays\" value=sun onclick=\"uncheckAll(this,'start_weekdays')\">%s",search(lpublic,"week_sun"));
					  fprintf(cgiOut,"<input type=\"checkbox\" style=margin-left:15px name=\"start_weekdays\" value=hebdomad onclick=\"checkAll(this,'start_weekdays')\">%s",search(lwlan,"all"));
					fprintf(cgiOut,"</td>"\
				"</tr>"\
				"<tr height=30>"\
					"<td>%s:</td>",search(lpublic,"stop_time"));
					fprintf(cgiOut,"<td><input name=stop_time size=15 maxLength=8 onkeypress=\"return event.keyCode>=48&&event.keyCode<=58\" value=\"\" onblur=\"isTime(this.value);\"><font color=red>(HH:MM:SS)</font></td>"\
					"<td>"\
					  "<select name=stop_timer_type id=stop_timer_type style=width:100px>"\
						"<option value=>"\
						"<option value=once>once"\
						"<option value=cycle>cycle"\
					  "</select></td>"\
				"</tr>"\
				"<tr height=30>"\
					"<td>&nbsp;</td>"\
					"<td colspan=2>"\
					  "<input type=\"checkbox\" name=\"stop_weekdays\" value=mon onclick=\"uncheckAll(this,'stop_weekdays')\">%s",search(lpublic,"week_mon"));
					  fprintf(cgiOut,"<input type=\"checkbox\" style=margin-left:15px name=\"stop_weekdays\" value=tue onclick=\"uncheckAll(this,'stop_weekdays')\">%s",search(lpublic,"week_tue"));
					  fprintf(cgiOut,"<input type=\"checkbox\" style=margin-left:15px name=\"stop_weekdays\" value=wed onclick=\"uncheckAll(this,'stop_weekdays')\">%s",search(lpublic,"week_wed"));
					  fprintf(cgiOut,"<input type=\"checkbox\" style=margin-left:15px name=\"stop_weekdays\" value=thu onclick=\"uncheckAll(this,'stop_weekdays')\">%s",search(lpublic,"week_thu"));
					fprintf(cgiOut,"</td>"\
				"</tr>"\
				"<tr height=30>"\
					"<td>&nbsp;</td>"\
					"<td colspan=2>"\
					  "<input type=\"checkbox\" name=\"stop_weekdays\" value=fri onclick=\"uncheckAll(this,'stop_weekdays')\">%s",search(lpublic,"week_fri"));
					  fprintf(cgiOut,"<input type=\"checkbox\" style=margin-left:15px name=\"stop_weekdays\" value=sat onclick=\"uncheckAll(this,'stop_weekdays')\">%s",search(lpublic,"week_sat"));
					  fprintf(cgiOut,"<input type=\"checkbox\" style=margin-left:15px name=\"stop_weekdays\" value=sun onclick=\"uncheckAll(this,'stop_weekdays')\">%s",search(lpublic,"week_sun"));
					  fprintf(cgiOut,"<input type=\"checkbox\" style=margin-left:15px name=\"stop_weekdays\" value=hebdomad onclick=\"checkAll(this,'stop_weekdays')\">%s",search(lwlan,"all"));
					fprintf(cgiOut,"</td>"\
				"</tr>"\
                "<tr>"\
                "<td><input type=hidden name=encry_conwlanser value=%s></td>",m);
                fprintf(cgiOut,"<td><input type=hidden name=wlan_id value=%s></td>",n);	
                fprintf(cgiOut,"<td><input type=hidden name=wlan_name value=%s></td>",t);
	          fprintf(cgiOut,"</tr>"\
			  "<tr>"\
                "<td><input type=hidden name=page_no value=%s></td>",pn);
			    fprintf(cgiOut,"<td><input type=hidden name=instance_id value=%s></td>",ins_id);
				fprintf(cgiOut,"<td><input type=hidden name=SubmitFlag value=%d></td>",1);
	          fprintf(cgiOut,"</tr>");
		  }
		  else
		    fprintf(cgiOut,"<tr><td>%s</td></tr>",search(lpublic,"no_security"));	
		}
		else
          fprintf(cgiOut,"<tr><td>%s</td></tr>",search(lpublic,"contact_adm"));
  fprintf(cgiOut,"</table></td></tr>"\
"</table>"\
              "</td>"\
            "</tr>"\
            "<tr height=4 valign=top>"\
              "<td width=120 height=4 align=right valign=top><img src=/images/bottom_07.gif width=1 height=10/></td>"\
              "<td width=827 height=4 valign=top bgcolor=#FFFFFF><img src=/images/bottom_06.gif width=827 height=15/></td>"\
            "</tr>"\
          "</table>"\
        "</td>"\
        "<td width=15 background=/images/di999.jpg>&nbsp;</td>"\
      "</tr>"\
    "</table></td>"\
  "</tr>"\
  "<tr>"\
    "<td colspan=3 align=left valign=top background=/images/di777.jpg><img src=/images/di555.jpg width=61 height=62/></td>"\
    "<td align=left valign=top background=/images/di777.jpg>&nbsp;</td>"\
    "<td align=left valign=top background=/images/di777.jpg><img src=/images/di666.jpg width=74 height=62/></td>"\
  "</tr>"\
"</table>"\
"</div>"\
"</form>"\
"</body>"\
"</html>");	  
if(result1==1)
{
  Free_one_wlan_head(whead);
}
if(result2 == 1)
{
  Free_security_head(head);
}
return 0;
}

void ConWlan(instance_parameter *ins_para,int id,int inter_num,int security_flag,struct list *lpublic,struct list *lwlan)
{
	int i = 0,flag = 1,result = cgiFormNotFound,del_inf_flag = 1,ret = 0,bind_result = 1;
	char **del_inf_responses;  
	char temp[100] = { 0 };
	char security_id[10] = { 0 };
	char *endptr = NULL;  
	int sec_id = 0;
    char max_sec_num[10] = { 0 };
	char **responses;  	
	char hessid[5] = { 0 };
	char nasid_num[130] = { 0 };	
	char maxnum[10] = { 0 };	
	int MNUM = 0;
	char type[10] = { 0 };	
	char para[10] = { 0 };
	int status = 0;
	char command[255] = { 0 };
	char uplink_ip[20] = { 0 };
	char ip1[4] = { 0 };
	char ip2[4] = { 0 };
	char ip3[4] = { 0 };
	char ip4[4] = { 0 };
	char uplink_state[10] = { 0 };
	char multi_user_switch[10] = { 0 };
	char wlan_id[10] = { 0 };
	char max_wlan_num[10] = { 0 };
	char stat[10] = { 0 };
	char new_essid[35] = { 0 };	
	char uplink_threshold[DEFAULT_LEN] = { 0 };
	char downlink_threshold[DEFAULT_LEN] = { 0 };
	char tmp[10] = { 0 };
	char timer_type[15] = { 0 };
	char timer_state[10] = { 0 };
	char start_time[10] = { 0 };
	char start_timer_type[10] = { 0 };
	int start_argv_num=0;
	char **start_weekdays;
	char stop_time[10] = { 0 };
	char stop_timer_type[10] = { 0 };
	int stop_argv_num=0;
	char **stop_weekdays;
	
	/************* delete binding interface ******************/
	result = cgiFormStringMultiple("del_bind_inter", &del_inf_responses);
    if(result != cgiFormNotFound)	
    {
	  i = 0;
	  while((del_inf_responses[i])&&(del_inf_flag==1))
	  {
	    ret=wlan_delete_interface(ins_para->parameter,ins_para->connection,id,del_inf_responses[i]); /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwlan doesn't binding this interface������-2��ʾWlan ID Not existed*/
																									 /*����-3��ʾInterface not existed������-4��ʾWlan is enable binding error! please disable first������-5��ʾerror*/
	    switch(ret)
	    {
	      case SNMPD_CONNECTION_ERROR:
		  case 0:{
				   ShowAlert(search(lwlan,"delete_inter_fail"));
				   del_inf_flag=0;
				   break;
			     }
		  case 1:break;
		  case -1:{
				    ShowAlert(search(lwlan,"wlan_dont_bind_inter"));
				    del_inf_flag=0;
				    break;
				  } 
		  case -2:{
				    ShowAlert(search(lwlan,"wlan_not_exist"));
				    del_inf_flag=0;
				    break;
				  }
		  case -3:{
				    ShowAlert(search(lpublic,"interface_not_exist"));
				    del_inf_flag=0;
				    break;
				  }
		  case -4:{
				    ShowAlert(search(lwlan,"dis_wlan"));
				    del_inf_flag=0;
				    break;
				  }
		  case -5:{
				    ShowAlert(search(lpublic,"error"));
				    del_inf_flag=0;
				    break;
				  }
	    }
	    i++;
	  }
	  cgiStringArrayFree(del_inf_responses);
	  
	  if(del_inf_flag==0)
		flag=0;
    }

	

    /************* binding security ******************/	
	memset(security_id,0,sizeof(security_id));
	cgiFormStringNoNewlines("sec_id",security_id,10); 
	if(strcmp(security_id,""))
	{
		sec_id= strtoul(security_id,&endptr,10);
 		ret=apply_wlanID(ins_para->parameter,ins_para->connection,sec_id,id);	
		switch(ret)
		{
		  case SNMPD_CONNECTION_ERROR:
		  case 0:ShowAlert(search(lwlan,"bind_secur_fail"));		   /*�󶨰�ȫ����ʧ��*/
				 flag=0;
				 break;
		  case 1:break;
		  case -1:ShowAlert(search(lwlan,"wlan_not_exist"));		  /*wlan id not exist*/
				  flag=0;
				  break;	   
		  case -2:ShowAlert(search(lpublic,"no_security")); 		  /*asd security profile not exist*/
				  flag=0;
				  break;
		  case -3:ShowAlert(search(lwlan,"secur_prof_not_intg"));	  /*asd security profile not integrity*/
				  flag=0;
				  break;
		  case -4:ShowAlert(search(lpublic,"encry_type_not_match"));   /*encryption type dosen't match with security type*/
				  flag=0;
				  break;
		  case -5:ShowAlert(search(lwlan,"dis_wlan"));				  /*should be disable wlan first*/
				  flag=0;
				  break;
		  case -6:{
					memset(temp,0,sizeof(temp));
					strncpy(temp,search(lpublic,"secur_id_illegal1"),sizeof(temp)-1);
					memset(max_sec_num,0,sizeof(max_sec_num));
					snprintf(max_sec_num,sizeof(max_sec_num)-1,"%d",WLAN_NUM-1);
					strncat(temp,max_sec_num,sizeof(temp)-strlen(temp)-1);
					strncat(temp,search(lpublic,"secur_id_illegal2"),sizeof(temp)-strlen(temp)-1);
					ShowAlert(temp);
					flag=0;
					break;
				  }
		  case -7:{
					memset(temp,0,sizeof(temp));
					strncpy(temp,search(lwlan,"wlan_id_illegal1"),sizeof(temp)-1);
					memset(max_wlan_num,0,sizeof(max_wlan_num));
					snprintf(max_wlan_num,sizeof(max_wlan_num)-1,"%d",WLAN_NUM-1);
					strncat(temp,max_wlan_num,sizeof(temp)-strlen(temp)-1);
					strncat(temp,search(lwlan,"wlan_id_illegal2"),sizeof(temp)-strlen(temp)-1);
					ShowAlert(temp);
					flag=0;
					break;
				  }
		  case -8:ShowAlert(search(lwlan,"security_rdc_not_config"));/*security rdc has not config*/
				  flag=0;
				  break;
		}	
	}
	

	/************* binding interface ******************/	
	result = cgiFormStringMultiple("bind_interface", &responses);
	if(result != cgiFormNotFound)
	{
	   i = 0;	
	   while((responses[i])&&(bind_result==1))
	   {
		 ret=wlan_apply_interface(ins_para->parameter,ins_para->connection,id,responses[i]); 
		   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾthe length of interface name excel 16*/
	       /*����-2��ʾwlan id does not exist������-3��ʾinterface dose not exist*/
	       /*����-4��ʾwlan is enable,please disable it first������-5��ʾerror������-6ʾWLAN ID�Ƿ�*/
		   /*����-7��ʾis no local interface, permission denial������-8��ʾinterface has be binded in other hansi*/
		   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
		 switch(ret)
		 {
		   case SNMPD_CONNECTION_ERROR:
		   case 0:{
					bind_result=0;
					ShowAlert(search(lwlan,"bind_interface_fail"));  	/*�󶨽ӿ�ʧ��*/
					break;
				  }
		   case 1:break;											  	/*�󶨽ӿڳɹ�*/
		   case -1:{
					 bind_result=0;
					 ShowAlert(search(lpublic,"interface_too_long"));	/*the length of interface name excel 16*/
					 break; 
				   }
		   case -2:{
					 bind_result=0;
					 ShowAlert(search(lwlan,"wlan_not_exist")); 		/*wlan id does not exist*/
					 break; 
				   }
		   case -3:{
					 bind_result=0;
					 ShowAlert(search(lpublic,"interface_not_exist")); 	/*interface dose not exist*/
					 break; 
				   }
		   case -4:{
					 bind_result=0;
					 ShowAlert(search(lwlan,"dis_wlan"));				/*wlan is enable,please disable it first*/
					 break; 
				   }
		   case -5:{
					 bind_result=0;
					 ShowAlert(search(lpublic,"error"));				/*error*/
					 break;  
				   }
		   case -6:{
		   			 bind_result=0;
				     memset(temp,0,sizeof(temp));
				     strncpy(temp,search(lwlan,"wlan_id_illegal1"),sizeof(temp)-1);
				     memset(max_wlan_num,0,sizeof(max_wlan_num));
				     snprintf(max_wlan_num,sizeof(max_wlan_num)-1,"%d",WLAN_NUM-1);
				     strncat(temp,max_wlan_num,sizeof(temp)-strlen(temp)-1);
				     strncat(temp,search(lwlan,"wlan_id_illegal2"),sizeof(temp)-strlen(temp)-1);
				     ShowAlert(temp);
				     break;
				   }
		   case -7:{
		   			 bind_result=0;
				     ShowAlert(search(lpublic,"no_local_interface"));
				     break;
				   }
		   case -8:{
		   			 bind_result=0;
				     ShowAlert(search(lwlan,"interface_bind_in_other_hansi"));
				     break;
				   }
		 }
		 i++;
	   }
	   cgiStringArrayFree(responses);

	   if(bind_result==0) 
	     flag=0;
	}
	else if(inter_num==0)/*WLANû�а󶨽ӿ�*/
	{
	  memset(stat,0,sizeof(stat));
	  cgiFormStringNoNewlines("wlan_service",stat,10); 
	  if(strcmp(stat,""))
	  {
		ShowAlert(search(lpublic,"select_interface"));
		flag=0;
	  }
	}


	/*********************************************************/
	/** 				         �޸�ESSID      					**/
	/*********************************************************/
	memset(new_essid,0,sizeof(new_essid));
	cgiFormStringNoNewlines("new_essid",new_essid,35); 
	if(strcmp(new_essid,""))
	{
		if(strchr(new_essid,' ')==NULL)/*�������ո�*/
		{
			ret=set_wlan_ascii_essid_cmd(ins_para->parameter,ins_para->connection,id,(unsigned char *)new_essid); 
			switch(ret)
			{
			   case SNMPD_CONNECTION_ERROR:
			   case 0:ShowAlert(search(lwlan,"set_essid_fail"));	/*����essidʧ��*/
					  flag=0;
					  break;
			   case 1:break;
			   case -1:ShowAlert(search(lpublic,"input_too_long")); /*essid is too long,out of the limit of 64*/
					   flag=0;
					   break; 
			   case -2:ShowAlert(search(lwlan,"wlan_not_exist"));	/*wlan id does not exist*/
					   flag=0;
					   break; 
			   case -3:ShowAlert(search(lwlan,"dis_wlan")); 		/*WLAN is enable,please disable it first*/
					   flag=0;
					   break; 
			   
			   case -4:ShowAlert(search(lpublic,"error"));			/*error*/
					   flag=0;
					   break;  
			}
		}
		else
		{
			flag=0;
			ShowAlert(search(lpublic,"input_para_dont_contain_spaces"));
		}
	}

	/*********************************************************/
	/** 				         ����hideessid     					**/
	/*********************************************************/
	memset(hessid,0,sizeof(hessid));
	cgiFormStringNoNewlines("set_hessid",hessid,5); 
	if(strcmp(hessid,""))
	{
		ret=set_hideessid(ins_para->parameter,ins_para->connection,id,hessid);
		switch(ret)
		{
		   case SNMPD_CONNECTION_ERROR:
		   case 0:ShowAlert(search(lwlan,"set_hessid_fail"));	/*����Hideessidʧ��*/
				  flag=0;
				  break;
		   case 1:break;
		   case -1:ShowAlert(search(lwlan,"wlan_not_exist"));	/*wlan id not exist*/
				   flag=0;
				   break; 
		   case -2:ShowAlert(search(lwlan,"dis_wlan")); 		/*WLAN is enable,please disable it first*/
				   flag=0;
				   break; 
		   case -3:ShowAlert(search(lpublic,"error"));			/*error*/
				   flag=0;
				   break;  
		}
	}

	/*********************************************************/
	/** 				         ����NASID      					**/
	/*********************************************************/
	
    memset(nasid_num,0,sizeof(nasid_num));
    cgiFormStringNoNewlines("nasid",nasid_num,130);
	if(strcmp(nasid_num,"") != 0)
    {
 	 	result = cgiFormStringMultiple("bind_interface", &responses);
		if(result != cgiFormNotFound)
		{
			i = 0;	
			bind_result=1;
			while((responses[i])&&(bind_result==1))
			{
			  ret=set_interface_nasid_cmd(ins_para->parameter,ins_para->connection,id,responses[i],nasid_num); 
			  switch(ret)
			  {
			    case SNMPD_CONNECTION_ERROR:
				case 0:{
						 bind_result=0;
						 ShowAlert(search(lwlan,"bind_interface_fail"));	 /*�󶨽ӿ�ʧ��*/
						 break;
					   }
				case 1:break;											 /*�󶨽ӿڳɹ�*/
				case -1:{
						  bind_result=0;
						  ShowAlert(search(lpublic,"interface_too_long"));	 /*the length of interface name excel 16*/
						  break; 
						}
				case -2:{
						  bind_result=0;
						  ShowAlert(search(lwlan,"nasid_len")); 	  /*wlan id not exist*/
						  break; 
						}
				case -3:{
						  bind_result=0;
						  ShowAlert(search(lwlan,"nasid_unknow"));	/*Interface not existed*/
						  break; 
						}
				case -4:{
						  bind_result=0;
						  ShowAlert(search(lpublic,"interface_not_exist"));  /*Interface not existed*/
						  break; 
						}
				case -5:{
						  bind_result=0;
						  ShowAlert(search(lwlan,"inter_err")); 	   /*Wlan is enable binding error! please disable first!*/
						  break; 
						}
				case -6:{
						  bind_result=0;
						  ShowAlert(search(lwlan,"no_secur_bind")); 
						  break;	
						}
				case -7:{
						  bind_result=0;
						  ShowAlert(search(lwlan,"nasid_empty"));		/*no nas_id needed,please apply interface without nas_identifier*/
						  break;	
						}
				case -8:{
						  bind_result=0;
						  ShowAlert(search(lwlan,"wlan_bind_err")); 	/*wlan be enable,please service disable first*/
						  break;	
						}
				case -9:{
						  bind_result=0;
						  ShowAlert(search(lpublic,"error"));		   /*error*/
						  break;	
						}
				case -10:{
			   			   bind_result=0;
					       memset(temp,0,sizeof(temp));
					       strncpy(temp,search(lwlan,"wlan_id_illegal1"),sizeof(temp)-1);
					       memset(max_wlan_num,0,sizeof(max_wlan_num));
					       snprintf(max_wlan_num,sizeof(max_wlan_num)-1,"%d",WLAN_NUM-1);
					       strncat(temp,max_wlan_num,sizeof(temp)-strlen(temp)-1);
					       strncat(temp,search(lwlan,"wlan_id_illegal2"),sizeof(temp)-strlen(temp)-1);
					       ShowAlert(temp);
					       break;
					     }
				case -11:{
						   bind_result=0;
						   ShowAlert(search(lwlan,"interface_bind_in_other_hansi")); 	/*interface has be binded in other hansi*/
						   break;	
						 }
			  }
			  i++;
			}
			cgiStringArrayFree(responses);
			
			if(bind_result==0)
			  flag=0;
		}
		else
		{
		    ShowAlert(search(lwlan,"select_inter"));	
		    flag=0;
		}
	}
	

	/**********************************************************************/
	/** 				���������������sta�� 					 **/
	/*********************************************************************/
	memset(maxnum,0,sizeof(maxnum));
	cgiFormStringNoNewlines("max_num",maxnum,10);
	if(strcmp(maxnum,""))
	{
		MNUM= strtoul(maxnum,&endptr,10);
		if((MNUM>0)&&(MNUM<65537)) 
		{
			ret=config_wlan_max_sta_num(ins_para->parameter,ins_para->connection,id,maxnum);			
			switch (ret)
			{
			  case SNMPD_CONNECTION_ERROR:
			  case 0:{
					   memset(temp,0,sizeof(temp));
					   strncpy(temp,search(lwlan,"con_bss_max_sta_num_fail1"),sizeof(temp)-1);
					   strncat(temp,"WLAN",sizeof(temp)-strlen(temp)-1);
					   strncat(temp,search(lwlan,"con_bss_max_sta_num_fail2"),sizeof(temp)-strlen(temp)-1);
					   ShowAlert(temp);
					   flag=0;
				       break;
				     }
			  case 1:break;
			  case -1:ShowAlert(search(lpublic,"unknown_input"));   	/*unknown NUM format*/
					  flag=0;
					  break; 
			  case -2:ShowAlert(search(lwlan,"wlan_not_exist"));   		/*wlan id not exist*/
					  flag=0;
					  break; 
			  case -3:ShowAlert(search(lwlan,"more_sta_has_access")); 	/*more sta(s) has accessed before you set max sta num*/
					  flag=0;
		              break; 
			  case -4:ShowAlert(search(lpublic,"oper_fail"));		  	/*operation fail*/
					  flag=0;
					  break; 
			  case -5:ShowAlert(search(lpublic,"error"));		   		/*error*/
					  flag=0;
					  break;
			  case -6:{													/*WLAN ID�Ƿ�*/
				        memset(temp,0,sizeof(temp));
					    strncpy(temp,search(lwlan,"wlan_id_illegal1"),sizeof(temp)-1);
					    memset(max_wlan_num,0,sizeof(max_wlan_num));
					    snprintf(max_wlan_num,sizeof(max_wlan_num)-1,"%d",WLAN_NUM-1);
					    strncat(temp,max_wlan_num,sizeof(temp)-strlen(temp)-1);
					    strncat(temp,search(lwlan,"wlan_id_illegal2"),sizeof(temp)-strlen(temp)-1);
					    ShowAlert(temp);
					    flag=0;
					    break;
				      }
			  case -7:{													/*input max sta num should be 1-65536*/
			  			memset(temp,0,sizeof(temp));
					    strncpy(temp,search(lwlan,"max_sta_num_illegal3"),sizeof(temp)-1);
					    strncat(temp,"65536",sizeof(temp)-strlen(temp)-1);
					    strncat(temp,search(lwlan,"max_sta_num_illegal2"),sizeof(temp)-strlen(temp)-1);
					    ShowAlert(temp);
				        flag=0;
					    break;
			  		  }
			}
		}
		else
	  	{
		  memset(temp,0,sizeof(temp));
		  strncpy(temp,search(lwlan,"max_sta_num_illegal3"),sizeof(temp)-1);
		  strncat(temp,"65536",sizeof(temp)-strlen(temp)-1);
		  strncat(temp,search(lwlan,"max_sta_num_illegal2"),sizeof(temp)-strlen(temp)-1);
		  ShowAlert(temp);
	      flag=0;
		}
	}
	
   

	/********************************************************************/
	/** 				   ����WLAN���ؾ��⿪�� 				   ***/
	/*******************************************************************/
	memset(type,0,sizeof(type));
	cgiFormStringNoNewlines("wlan_balance_type",type,10);
	if(strcmp(type,"") != 0)
	{
		ret=config_wlan_load_balance(ins_para->parameter,ins_para->connection,id,type);
		switch (ret)
		{
		  case SNMPD_CONNECTION_ERROR:
		  case 0:ShowAlert(search(lwlan,"con_wlan_load_balance_fail"));
				 flag=0;
				 break;
		  case 1:break;
		  case -1:ShowAlert(search(lwlan,"wlan_not_exist"));		/*wlan id not exist*/
				  flag=0;
				  break; 
		  case -2:ShowAlert(search(lpublic,"oper_fail"));			/*operation fail*/
				  flag=0;
				  break; 
		  case -3:ShowAlert(search(lpublic,"error"));				/*error*/
				  flag=0;
				  break;	
		}	
	}


	
	/*******************************************************************/
	/** 		����WLAN�������ĸ��ؾ��ⴥ��ֵ			  ***/
	/*******************************************************************/
	memset(para,0,sizeof(para));
	cgiFormStringNoNewlines("number_balance_value",para,10);
	if(strcmp(para,"") != 0)
	{
	  ret=config_wlan_number_balance_parameter(ins_para->parameter,ins_para->connection,id,para);	 
	  switch (ret)
	  {
	    case SNMPD_CONNECTION_ERROR:
		case 0:ShowAlert(search(lwlan,"con_number_triger_fail"));
			   flag=0;
			   break;
		case 1:break;
		case -1:ShowAlert(search(lwlan,"balance_number_iillegal"));  /*balance parameter should be 1 to 10*/
				flag=0;
				break; 
		case -2:ShowAlert(search(lwlan,"wlan_not_exist"));			 /*wlan id not exist*/
				flag=0;
				break; 
		case -3:ShowAlert(search(lpublic,"oper_fail")); 			 /*operation fail*/
				flag=0;
				break; 
		case -4:ShowAlert(search(lpublic,"error")); 				 /*error*/
				flag=0;
				break;	
	  } 	
	}


	/*******************************************************************/
	/** 		����WLAN�������ĸ��ؾ��ⴥ��ֵ			  ***/
	/*******************************************************************/
	memset(para,0,sizeof(para));
	cgiFormStringNoNewlines("flow_balance_value",para,10);
	if(strcmp(para,"") != 0)
	{
	  ret=config_wlan_flow_balance_parameter(ins_para->parameter,ins_para->connection,id,para);
	  switch (ret)
	  {
	    case SNMPD_CONNECTION_ERROR:
		case 0:ShowAlert(search(lwlan,"con_flow_triger_fail"));
			   flag=0;
			   break;
		case 1:break;
		case -1:ShowAlert(search(lwlan,"balance_flow_iillegal"));	/*balance parameter should be 1 to 30*/
				flag=0;
				break; 
		case -2:ShowAlert(search(lwlan,"wlan_not_exist"));			/*wlan id not exist*/
				flag=0;
				break; 
		case -3:ShowAlert(search(lpublic,"oper_fail")); 			/*operation fail*/
				flag=0;
				break; 
		case -4:ShowAlert(search(lpublic,"error")); 				/*error*/
				flag=0;
				break;	
	  } 	
	}

	/*******************************************************************/
	/** 					 ����WLAN�����ο���					  ***/
	/*******************************************************************/
	memset(type,0,sizeof(type));
	memset(para,0,sizeof(para));
	cgiFormStringNoNewlines("roam_type",type,10);
	cgiFormStringNoNewlines("roam_state",para,10);
	if((strcmp(type,"") != 0)&&(strcmp(para,"") != 0))
	{
	  ret=wlan_roam_policy(ins_para->parameter,ins_para->connection,id,type,para); 
	  switch (ret)
	  {
	    case SNMPD_CONNECTION_ERROR:
		case 0:ShowAlert(search(lwlan,"con_roam_switch_fail"));
			   flag=0;
			   break;
		case 1:break;
		case -1:ShowAlert(search(lwlan,"wlan_not_exist"));				/*wlan id not exist*/
				flag=0;
				break; 
		case -2:ShowAlert(search(lpublic,"oper_fail")); 				/*operation fail*/
				flag=0;
				break; 
		case -3:ShowAlert(search(lwlan,"dis_wlan"));					/* wlan should be disable first*/
				flag=0;
				break; 
		case -4:ShowAlert(search(lwlan,"dis_roaming")); 				/*roaming should be disable first*/
				flag=0;
				break;	
		case -5:ShowAlert(search(lpublic,"error")); 					/*error*/
				flag=0;
				break;	
	  } 	
	}


	/*******************************************************************/
	/** 				       ����WLAN����������ֵ	    	   		  ***/
	/*******************************************************************/
	memset(uplink_threshold,0,sizeof(uplink_threshold));
	cgiFormStringNoNewlines("wlan_uplink_traffic_limit_threshold",uplink_threshold,DEFAULT_LEN);
	if(strcmp(uplink_threshold,"") != 0)
	{
		ret=set_whole_wlan_traffic_limit_cmd(ins_para->parameter,ins_para->connection,id,uplink_threshold);
		switch(ret)
		{
			case SNMPD_CONNECTION_ERROR:
			case 0:ShowAlert(search(lwlan,"set_wlan_uplink_traffic_limit_threshold_fail"));
				   flag=0;
				   break;
			case 1:break;
			case -1:ShowAlert(search(lpublic,"input_para_error"));
					flag=0;
					break; 
			case -2:ShowAlert(search(lwlan,"wlan_not_exist"));
					flag=0;
					break; 
			case -3:ShowAlert(search(lpublic,"error"));
					flag=0;
					break;
			case -4:{													/*WLAN ID�Ƿ�*/
				        memset(temp,0,sizeof(temp));
					    strncpy(temp,search(lwlan,"wlan_id_illegal1"),sizeof(temp)-1);
					    memset(max_wlan_num,0,sizeof(max_wlan_num));
					    snprintf(max_wlan_num,sizeof(max_wlan_num)-1,"%d",WLAN_NUM-1);
					    strncat(temp,max_wlan_num,sizeof(temp)-strlen(temp)-1);
					    strncat(temp,search(lwlan,"wlan_id_illegal2"),sizeof(temp)-strlen(temp)-1);
					    ShowAlert(temp);
					    flag=0;
					    break;
				      }
			case -5:ShowAlert(search(lpublic,"input_para_illegal"));
					flag=0;
					break;
		}
	}

	/*******************************************************************/
	/** 				       ����WLAN����������ֵ	    	   		  ***/
	/*******************************************************************/
	memset(downlink_threshold,0,sizeof(downlink_threshold));
	cgiFormStringNoNewlines("wlan_downlink_traffic_limit_threshold",downlink_threshold,DEFAULT_LEN);
	if(strcmp(downlink_threshold,"") != 0)
	{
		ret=set_whole_wlan_send_traffic_limit_cmd(ins_para->parameter,ins_para->connection,id,downlink_threshold);
		switch(ret)
		{
			case SNMPD_CONNECTION_ERROR:
			case 0:ShowAlert(search(lwlan,"set_wlan_downlink_traffic_limit_threshold_fail"));
				   flag=0;
				   break;
			case 1:break;
			case -1:ShowAlert(search(lpublic,"input_para_error"));
					flag=0;
					break; 
			case -2:ShowAlert(search(lwlan,"wlan_not_exist"));
					flag=0;
					break; 
			case -3:ShowAlert(search(lpublic,"error"));
					flag=0;
					break;
			case -4:{													/*WLAN ID�Ƿ�*/
				        memset(temp,0,sizeof(temp));
					    strncpy(temp,search(lwlan,"wlan_id_illegal1"),sizeof(temp)-1);
					    memset(max_wlan_num,0,sizeof(max_wlan_num));
					    snprintf(max_wlan_num,sizeof(max_wlan_num)-1,"%d",WLAN_NUM-1);
					    strncat(temp,max_wlan_num,sizeof(temp)-strlen(temp)-1);
					    strncat(temp,search(lwlan,"wlan_id_illegal2"),sizeof(temp)-strlen(temp)-1);
					    ShowAlert(temp);
					    flag=0;
					    break;
				      }
			case -5:ShowAlert(search(lpublic,"input_para_illegal"));
					flag=0;
					break;
		}
	}
	


	/*******************************************************************/
	/** 				       	       ����WDS����		    	   		  ***/
	/*******************************************************************/
	memset(para,0,sizeof(para));
	cgiFormStringNoNewlines("wds_state",para,10);
	if(strcmp(para,"") != 0)
	{
		ret=config_wds_service_cmd_func(ins_para->parameter,ins_para->connection,id,"wds",para);
		switch(ret)
		{
			case SNMPD_CONNECTION_ERROR:
			case 0:ShowAlert(search(lwlan,"set_wds_state_fail"));
				   flag=0;
				   break;
			case 1:break;
			case -1:ShowAlert(search(lwlan,"bind_interface_first"));
					flag=0;
					break; 
			case -2:ShowAlert(search(lwlan,"map_l3_inter_error"));
					flag=0;
					break; 
			case -3:ShowAlert(search(lwlan,"enable_wlan"));
					flag=0;
					break;
			case -4:ShowAlert(search(lpublic,"error"));
					flag=0;
					break;
			case -5:{													/*WLAN ID�Ƿ�*/
			          memset(temp,0,sizeof(temp));
				      strncpy(temp,search(lwlan,"wlan_id_illegal1"),sizeof(temp)-1);
				      memset(max_wlan_num,0,sizeof(max_wlan_num));
				      snprintf(max_wlan_num,sizeof(max_wlan_num)-1,"%d",WLAN_NUM-1);
				      strncat(temp,max_wlan_num,sizeof(temp)-strlen(temp)-1);
				      strncat(temp,search(lwlan,"wlan_id_illegal2"),sizeof(temp)-strlen(temp)-1);
				      ShowAlert(temp);
				      flag=0;
				      break;
			        }
		}
	}


	/*******************************************************************/
	/** 				       	����������·���      	   		  ***/
	/*******************************************************************/
	memset(uplink_ip,0,sizeof(uplink_ip));                                 
    memset(ip1,0,sizeof(ip1));
    cgiFormStringNoNewlines("ip1",ip1,4);	
    strncat(uplink_ip,ip1,sizeof(uplink_ip)-strlen(uplink_ip)-1);
    strncat(uplink_ip,".",sizeof(uplink_ip)-strlen(uplink_ip)-1);
    memset(ip2,0,sizeof(ip2));
    cgiFormStringNoNewlines("ip2",ip2,4); 
    strncat(uplink_ip,ip2,sizeof(uplink_ip)-strlen(uplink_ip)-1);	
    strncat(uplink_ip,".",sizeof(uplink_ip)-strlen(uplink_ip)-1);
    memset(ip3,0,sizeof(ip3));
    cgiFormStringNoNewlines("ip3",ip3,4); 
    strncat(uplink_ip,ip3,sizeof(uplink_ip)-strlen(uplink_ip)-1);	
    strncat(uplink_ip,".",sizeof(uplink_ip)-strlen(uplink_ip)-1);
    memset(ip4,0,sizeof(ip4));
    cgiFormStringNoNewlines("ip4",ip4,4);
    strncat(uplink_ip,ip4,sizeof(uplink_ip)-strlen(uplink_ip)-1);

	snprintf(wlan_id,sizeof(wlan_id)-1,"%d",id);
	
	memset(uplink_state,0,sizeof(uplink_state));
	cgiFormStringNoNewlines("uplink_detect_state",uplink_state,10);
	if((strcmp(uplink_ip,""))&&(strcmp(uplink_state,"")))
	{
		#if 0
		reset_sigmask();
		ret=uplink_detect_cmd_func(ins_para->parameter,ins_para->connection,uplink_ip,wlan_id,uplink_state);
		switch(ret)
		{
			case 0:ShowAlert(search(lwlan,"uplink_detect_fail"));
				   flag=0;
				   break;
			case 1:break;
			case -1:ShowAlert(search(lpublic,"unknown_ip_format"));
					flag=0;
					break; 
			case -2:ShowAlert(search(lpublic,"unknown_id_format"));
					flag=0;
					break; 
			case -3:{
				      memset(temp,0,sizeof(temp));
					  strncpy(temp,search(lwlan,"wlan_id_illegal1"),sizeof(temp)-1);
					  memset(max_wlan_num,0,sizeof(max_wlan_num));
					  snprintf(max_wlan_num,sizeof(max_wlan_num)-1,"%d",WLAN_NUM-1);
					  strncat(temp,max_wlan_num,sizeof(temp)-strlen(temp)-1);
					  strncat(temp,search(lwlan,"wlan_id_illegal2"),sizeof(temp)-strlen(temp)-1);
					  ShowAlert(temp);
					  flag=0;
					  break;
				    }
			case -4:ShowAlert(search(lwlan,"wlan_not_exist"));
					flag=0;
					break;
			case -5:ShowAlert(search(lpublic,"input_exceed_max_value"));
					flag=0;
					break;
			case -6:ShowAlert(search(lpublic,"input_para_illegal"));
					flag=0;
					break;
		}
		#endif

		ret = WID_Check_IP_Format((char*)uplink_ip);
		if(ret != WID_DBUS_SUCCESS){
			flag = 0;
			ShowAlert(search(lpublic,"unknown_ip_format"));
		}
		else
		{
			memset(command,0,sizeof(command));
			strncpy(command,"ccgi_uplink_detect.sh ",sizeof(command)-1);
			
			memset(tmp,0,sizeof(tmp));
			snprintf(tmp,sizeof(tmp)-1,"%d",ins_para->parameter.slot_id);
			strncat(command,tmp,sizeof(command)-strlen(command)-1);
			
			strncat(command," ",sizeof(command)-strlen(command)-1);
			memset(tmp,0,sizeof(tmp));
			snprintf(tmp,sizeof(tmp)-1,"%d",ins_para->parameter.local_id);
			strncat(command,tmp,sizeof(command)-strlen(command)-1);
			
			strncat(command," ",sizeof(command)-strlen(command)-1);
			memset(tmp,0,sizeof(tmp));
			snprintf(tmp,sizeof(tmp)-1,"%d",ins_para->parameter.instance_id);
			strncat(command,tmp,sizeof(command)-strlen(command)-1);
			
			strncat(command," ",sizeof(command)-strlen(command)-1);
			strncat(command,uplink_ip,sizeof(command)-strlen(command)-1);
			strncat(command," ",sizeof(command)-strlen(command)-1);
			strncat(command,wlan_id,sizeof(command)-strlen(command)-1);
			strncat(command," ",sizeof(command)-strlen(command)-1);
			strncat(command,uplink_state,sizeof(command)-strlen(command)-1);
			strncat(command,">/dev/null 2>&1",sizeof(command)-strlen(command)-1);
			status = system(command);	 
			ret = WEXITSTATUS(status);
			if(0!=ret)	  /*command fail*/
			{
				flag = 0;
				ShowAlert(search(lwlan,"uplink_detect_fail"));
			}
		}	
	}

	
	/*******************************************************************/
	/** 				       	 ���ö��û�����		    	   		  ***/
	/*******************************************************************/
	memset(multi_user_switch,0,sizeof(multi_user_switch));
	cgiFormStringNoNewlines("multi_user_switch",multi_user_switch,10);
	if(strcmp(multi_user_switch,"") != 0)
	{
		ret=set_wlan_bss_multi_user_optimize_cmd(ins_para->parameter,ins_para->connection,id,multi_user_switch);
		switch(ret)
		{
			case 0:ShowAlert(search(lwlan,"set_multi_user_switch_fail"));
				   flag=0;
				   break;
			case 1:break;
			case -1:{
				      memset(temp,0,sizeof(temp));
				      strncpy(temp,search(lwlan,"wlan_id_illegal1"),sizeof(temp)-1);
				      memset(max_wlan_num,0,sizeof(max_wlan_num));
				      snprintf(max_wlan_num,sizeof(max_wlan_num)-1,"%d",WLAN_NUM-1);
				      strncat(temp,max_wlan_num,sizeof(temp)-strlen(temp)-1);
				      strncat(temp,search(lwlan,"wlan_id_illegal2"),sizeof(temp)-strlen(temp)-1);
				      ShowAlert(temp);
					  flag=0;
				      break;
				    }
			case -2:ShowAlert(search(lwlan,"bss_not_exist"));
					flag=0;
					break; 
			case -3:ShowAlert(search(lpublic,"oper_fail"));
					flag=0;
					break; 
			case -4:ShowAlert(search(lwlan,"rad_dont_bind_wlan"));
					flag=0;
					break;
			case -5:ShowAlert(search(lpublic,"error"));
					flag=0;
					break;
		}
	}
		
	/*******************************************************************/
	/** 			     	       �������ӿ�ʼʱ��	       	   		  ***/
	/*******************************************************************/
	memset(start_time,0,sizeof(start_time));
	cgiFormStringNoNewlines("start_time",start_time,10);
	memset(start_timer_type,0,sizeof(start_timer_type));
	cgiFormStringNoNewlines("start_timer_type",start_timer_type,10);
    result = cgiFormStringMultiple("start_weekdays", &start_weekdays);
    if (result != cgiFormNotFound) 
    {
        int i = 0;
        while (start_weekdays[i]) 
		{
        	i++;
    	}
		start_argv_num = i;
    }	
	if((strcmp(start_time,"") != 0)&&(strcmp(start_timer_type,"") != 0)&&(start_argv_num>0)&&(result != cgiFormNotFound))
	{
	    if(start_argv_num == 8)/*ѡ��һ��*/
	    {
	    	start_argv_num-- ;
	    }
		ret=set_wlan_servive_timer_func_cmd(ins_para->parameter,ins_para->connection,id,"start",start_time,start_timer_type,start_argv_num,start_weekdays);
																						/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																						/*����-1��ʾinput patameter only with 'start' or 'stop'*/
																						/*����-2��ʾinput patameter format should be 12:32:56*/
																						/*����-3��ʾinput patameter only with 'once' or 'cycle'*/
																						/*����-4��ʾweekdays you want (like Sun Mon Tue Wed Thu Fri Sat or hebdomad)*/																																	  
																						/*����-5��ʾerror������-6ʾWLAN ID�Ƿ�*/
																						/*����-7��ʾthe starttimer or stoptimer should be disabled*/
																						/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
		switch(ret)
		{
			case SNMPD_CONNECTION_ERROR:
			case 0:ShowAlert(search(lwlan,"set_wlan_start_timer_fail"));
				   flag=0;
				   break;
			case 1:break;
			case -1:ShowAlert(search(lpublic,"input_para_error"));
					flag=0;
					break; 
			case -2:ShowAlert(search(lpublic,"timewrong"));
					flag=0;
					break; 
			case -3:ShowAlert(search(lpublic,"input_para_error"));
					flag=0;
					break; 
			case -4:ShowAlert(search(lpublic,"input_para_error"));
					flag=0;
					break; 
			case -5:ShowAlert(search(lpublic,"error"));
					flag=0;
					break;
			case -6:{
				      memset(temp,0,sizeof(temp));
				      strncpy(temp,search(lwlan,"wlan_id_illegal1"),sizeof(temp)-1);
				      memset(max_wlan_num,0,sizeof(max_wlan_num));
				      snprintf(max_wlan_num,sizeof(max_wlan_num)-1,"%d",WLAN_NUM-1);
				      strncat(temp,max_wlan_num,sizeof(temp)-strlen(temp)-1);
				      strncat(temp,search(lwlan,"wlan_id_illegal2"),sizeof(temp)-strlen(temp)-1);
				      ShowAlert(temp);
					  flag=0;
				      break;
				    }
			case -7:ShowAlert(search(lwlan,"starttimer_stoptimer_should_disabled"));
					flag=0;
					break;
		}
		cgiStringArrayFree(start_weekdays);
	}
	else if((strcmp(start_time,"") != 0)||(strcmp(start_timer_type,"") != 0)||(start_argv_num>0)||(result != cgiFormNotFound))  /*������ȫ*/
	{
		cgiStringArrayFree(start_weekdays);
		flag=0;
		ShowAlert(search(lpublic,"para_incom"));
	}

	/*******************************************************************/
	/** 			     	       �������ӽ���ʱ��	       	   		  ***/
	/*******************************************************************/
	memset(stop_time,0,sizeof(stop_time));
	cgiFormStringNoNewlines("stop_time",stop_time,10);
	memset(stop_timer_type,0,sizeof(stop_timer_type));
	cgiFormStringNoNewlines("stop_timer_type",stop_timer_type,10);
    result = cgiFormStringMultiple("stop_weekdays", &stop_weekdays);
    if (result != cgiFormNotFound) 
    {
        int i = 0;
        while (stop_weekdays[i]) 
		{
        	i++;
    	}
		stop_argv_num = i;
    }	
	if((strcmp(stop_time,"") != 0)&&(strcmp(stop_timer_type,"") != 0)&&(stop_argv_num>0)&&(result != cgiFormNotFound))
	{
	    if(stop_argv_num == 8)/*ѡ��һ��*/
	    {
	    	stop_argv_num-- ;
	    }
		ret=set_wlan_servive_timer_func_cmd(ins_para->parameter,ins_para->connection,id,"stop",stop_time,stop_timer_type,stop_argv_num,stop_weekdays);
																						/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																						/*����-1��ʾinput patameter only with 'start' or 'stop'*/
																						/*����-2��ʾinput patameter format should be 12:32:56*/
																						/*����-3��ʾinput patameter only with 'once' or 'cycle'*/
																						/*����-4��ʾweekdays you want (like Sun Mon Tue Wed Thu Fri Sat or hebdomad)*/																																	  
																						/*����-5��ʾerror������-6ʾWLAN ID�Ƿ�*/
																						/*����-7��ʾthe starttimer or stoptimer should be disabled*/
																						/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
		switch(ret)
		{
			case SNMPD_CONNECTION_ERROR:
			case 0:ShowAlert(search(lwlan,"set_wlan_stop_timer_fail"));
				   flag=0;
				   break;
			case 1:break;
			case -1:ShowAlert(search(lpublic,"input_para_error"));
					flag=0;
					break; 
			case -2:ShowAlert(search(lpublic,"timewrong"));
					flag=0;
					break; 
			case -3:ShowAlert(search(lpublic,"input_para_error"));
					flag=0;
					break; 
			case -4:ShowAlert(search(lpublic,"input_para_error"));
					flag=0;
					break; 
			case -5:ShowAlert(search(lpublic,"error"));
					flag=0;
					break;
			case -6:{
				      memset(temp,0,sizeof(temp));
				      strncpy(temp,search(lwlan,"wlan_id_illegal1"),sizeof(temp)-1);
				      memset(max_wlan_num,0,sizeof(max_wlan_num));
				      snprintf(max_wlan_num,sizeof(max_wlan_num)-1,"%d",WLAN_NUM-1);
				      strncat(temp,max_wlan_num,sizeof(temp)-strlen(temp)-1);
				      strncat(temp,search(lwlan,"wlan_id_illegal2"),sizeof(temp)-strlen(temp)-1);
				      ShowAlert(temp);
					  flag=0;
				      break;
				    }
			case -7:ShowAlert(search(lwlan,"starttimer_stoptimer_should_disabled"));
					flag=0;
					break;
		}
		cgiStringArrayFree(stop_weekdays);
	}
	else if((strcmp(stop_time,"") != 0)||(strcmp(stop_timer_type,"") != 0)||(stop_argv_num>0)||(result != cgiFormNotFound))   /*������ȫ*/
	{
		cgiStringArrayFree(start_weekdays);
		flag=0;
		ShowAlert(search(lpublic,"para_incom"));
	}
	
	
	/*******************************************************************/
	/** 				      	       �������ӿ���		       	   		  ***/
	/*******************************************************************/
	memset(timer_type,0,sizeof(timer_type));
	cgiFormStringNoNewlines("timer_type",timer_type,15);
	memset(timer_state,0,sizeof(timer_state));
	cgiFormStringNoNewlines("timer_state",timer_state,10);
	if((strcmp(timer_type,"") != 0)&&(strcmp(timer_state,"") != 0))
	{
		ret=set_wlan_timer_able_cmd(ins_para->parameter,ins_para->connection,id,timer_type,timer_state);
																						/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																						/*����-1��ʾfirst input patameter only with 'starttimer' or 'stoptimer'*/
																						/*����-2��ʾsecond input patameter only with 'enable' or 'disable'*/
																						/*����-3��ʾerror������-4ʾWLAN ID�Ƿ�*/
																						/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
		switch(ret)
		{
			case SNMPD_CONNECTION_ERROR:
			case 0:ShowAlert(search(lwlan,"set_wlan_timer_fail"));
				   flag=0;
				   break;
			case 1:break;
			case -1:ShowAlert(search(lpublic,"input_para_error"));
					flag=0;
					break; 
			case -2:ShowAlert(search(lpublic,"input_para_error"));
					flag=0;
					break; 
			case -3:ShowAlert(search(lpublic,"error"));
					flag=0;
					break;
			case -4:{
				      memset(temp,0,sizeof(temp));
				      strncpy(temp,search(lwlan,"wlan_id_illegal1"),sizeof(temp)-1);
				      memset(max_wlan_num,0,sizeof(max_wlan_num));
				      snprintf(max_wlan_num,sizeof(max_wlan_num)-1,"%d",WLAN_NUM-1);
				      strncat(temp,max_wlan_num,sizeof(temp)-strlen(temp)-1);
				      strncat(temp,search(lwlan,"wlan_id_illegal2"),sizeof(temp)-strlen(temp)-1);
				      ShowAlert(temp);
					  flag=0;
				      break;
				    }
		}
	}
	else if((strcmp(timer_type,"") != 0)||(strcmp(timer_state,"") != 0))  /*������ȫ*/
	{
		cgiStringArrayFree(start_weekdays);
		flag=0;
		ShowAlert(search(lpublic,"para_incom"));
	}

	
	/************* config wlan service ******************/	
	memset(stat,0,sizeof(stat));
	cgiFormStringNoNewlines("wlan_service",stat,10); 
	if(strcmp(stat,""))
	{
	  ret=config_wlan_service(ins_para->parameter,ins_para->connection,id,stat); /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾno security profile binded������-2��ʾwtp interface policy conflict*/
										                                         /*����-3��ʾyou map layer3 interace error������-4��ʾyou should bind interface first������-5��ʾerror*/
																				 /*����-7��ʾwlan bingding securithindex same*/
	  switch(ret)
	  {
	    case SNMPD_CONNECTION_ERROR:
		case 0:ShowAlert(search(lwlan,"con_wlan_ser_fail"));
			   flag=0;
			   break;
		case 1:break;
		case -1:ShowAlert(search(lwlan,"no_secur_bind"));
				flag=0;
				break;
		case -2:ShowAlert(search(lwlan,"wtp_ifpolicy_conflict"));
				flag=0;
				break;
		case -3:ShowAlert(search(lwlan,"map_l3_inter_error"));
				flag=0;
				break;
		case -4:ShowAlert(search(lwlan,"bind_interface_first"));
				flag=0;
				break;
		case -5:ShowAlert(search(lpublic,"error"));
				flag=0;
				break;
		case -7:ShowAlert(search(lwlan,"secIndex_is_same_with_other"));
				flag=0;
				break;
	  } 	  
	}

	if(flag==1)
	  ShowAlert(search(lpublic,"oper_succ"));
}


