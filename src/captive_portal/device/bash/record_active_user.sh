#!/bin/bash

source cp_start.sh

if [ ! $# -eq 3 ] ; then
	echo "Usage: record_active_user.sh add(del) id ip"
	exit
fi

#��ӡ�ɾ������֤�û�ʱ����Ϣ��¼��һ���ļ�


CP_RECORD_ACTION=$1
CP_RECORD_ID=$2
CP_RECORD_IP=$3

#�ļ���һ����ͨ��������֤����
#�ļ�ûһ�ж���һ����¼��ÿ����¼���ڼ�¼�˿�ͨ��ip��id��

#�ļ������ڣ������ļ�
[ -d /var/run/cpp ] || mkdir /var/run/cpp
[ -e $CP_RECORD_DB ] || touch $CP_RECORD_DB

#�õ�����
CP_ACTIVE_USER_NUM=$(cat $CP_RECORD_DB | head -1)

if [ ! -n "${CP_ACTIVE_USER_NUM}" ] ; then
	CP_ACTIVE_USER_NUM=0
	echo ${CP_ACTIVE_USER_NUM} > ${CP_RECORD_DB}
fi


CP_CUR_ACTIVE_USER=${CP_RECORD_ID}"#"${CP_RECORD_IP};
#�Ȳ��Ҹü�¼�Ƿ��Ѿ�����
RECORD_TEMP=`grep "^${CP_CUR_ACTIVE_USER}$" ${CP_RECORD_DB}`

#�����һ���û�
if [ ${CP_RECORD_ACTION} == 'add' ] ; then
	if [ ! -n "${RECORD_TEMP}" ] ; then
	#��ӵ�ĩβ
		echo ${CP_CUR_ACTIVE_USER} >> ${CP_RECORD_DB}
	#�����Ӽ�  #ע�⣬exprִ�������ʱ�������ǰ��Ҫ�� �ո񣬷���������ִ��
		CP_ACTIVE_USER_NUM=$(expr ${CP_ACTIVE_USER_NUM} + 1)
	fi
fi

#��ɾ��һ����
if [ ${CP_RECORD_ACTION} == 'del' ];then
	#���ip�Ƿ����
	if [ -n "${RECORD_TEMP}" ] ; then
		#ɾ����ip
		sed "/^${CP_CUR_ACTIVE_USER}$/d" ${CP_RECORD_DB} > ${CP_RECORD_DB}_temp;mv ${CP_RECORD_DB}_temp ${CP_RECORD_DB}
		#��������
		CP_ACTIVE_USER_NUM=$(expr ${CP_ACTIVE_USER_NUM} - 1)
		if [ ${CP_ACTIVE_USER_NUM} -lt 0 ];then
			CP_ACTIVE_USER_NUM=0
		fi
	fi
fi


#�޸�����
sed "1 c\\${CP_ACTIVE_USER_NUM}" ${CP_RECORD_DB} > ${CP_RECORD_DB}_temp; mv ${CP_RECORD_DB}_temp ${CP_RECORD_DB}

