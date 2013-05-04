#!/bin/bash

source cp_start.sh

if [ ! $# -eq 1 ] ; then
	echo "Usage: portal_get_flux.sh IPADDR"
	exit 1
fi

. portal_get_cpid.sh



IPADDR=$1

CP_FILTER_GROUP_AUTHORIZED=CP_${CP_ID}_GP_F_AUTH
CP_NAT_GROUP_AUTHORIZED=CP_${CP_ID}_GP_N_AUTH
CP_FILTER_AUTHORIZED=CP_${CP_ID}_F_AUTHORIZED_DEFAULT
CP_NAT_AUTHORIZED=CP_${CP_ID}_N_AUTHORIZED_DEFAULT



CP_CUR_IF=`get_user_interface ${IPADDR}`
if [ ${CP_CUR_IF} == "" ];then
#�û����ߵ�ʱ�����п��ܲ�������arp�У���Ҫpingһ���û���ʹ����arp(������������arp�ķ�����)
	ping.sh $IPADDR
	CP_CUR_IF=`get_user_interface ${IPADDR}`
	if [ ${CP_CUR_IF} == "" ];then
#�Ҳ������û�
		exit 2
	fi
fi

#��ã���������ֻ������ͨ��portal��֤���û�
if [ ${CP_CUR_MODE} -eq ${CP_MODE_IPTABLES} ];then

	iptables -nvxL ${CP_FILTER_GROUP_AUTHORIZED}_${CP_CUR_IF} | grep ${IPADDR}" " | sed "2,100d" | awk '{printf $2}'

else
	exit 3
fi
