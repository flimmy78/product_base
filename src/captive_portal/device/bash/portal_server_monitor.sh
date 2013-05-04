#/bin/bash
#��������portal server�Ƿ���ã���������ã��ͷ���trap�
#����ű���һ���ػ����̣�ÿ��һ��ʱ��֮�󣬾ͻ�ȥping��Ӧ��portal server ip��ֻpingһ�� ping����Ҳ��
#��һ�ε�ʱ����ȼ��֮ǰping����Ľ���������������ping�������ͨ���ͷ���trap��
#����ű��󲿷ֵ�ʱ�䶼����sleep������ռ��cpu̫��ʱ�䡣

source cp_start.sh

PORTAL_UNREACHABLE_IPS=""

if [ ! -f ${CP_DB_FILE} ];then
	exit
fi

while read line
do
	id=`echo $line | awk '{print $1}'`
	ip=`echo $line | awk '{print $2}'`

	ping.sh $ip >/dev/null 2>&1
	if [ ! $? == "0" ];then
		#��portal��ص���Ϣ
		if [ "${PORTAL_UNREACHABLE_IPS}" == "" ];then

			PORTAL_UNREACHABLE_IPS=${ip}
		else

			PORTAL_UNREACHABLE_IPS="${PORTAL_UNREACHABLE_IPS},${ip}"
		fi
	fi
done < ${CP_DB_FILE}

if [ ! "${PORTAL_UNREACHABLE_IPS}" == "" ];then
	printf ${PORTAL_UNREACHABLE_IPS}
fi
