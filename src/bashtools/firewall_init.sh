#!/bin/sh
#
###########################################################################
#
#              Copyright (C) Autelan Technology
#
#This software file is owned and distributed by Autelan Technology 
#
############################################################################
#THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
#ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
#WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
#DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR 
#ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
#(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
#LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
#ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
#(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
#SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
##############################################################################
#
# eag_init
#
# CREATOR:
# autelan.software.shaojunwu. team
# 
# DESCRIPTION: 
#     
#############################################################################

source cp_start.sh

echo "$0"



FIREWALL_DATA=/opt/services/conf/firewall_conf.conf
FIREWALL_STATUS=/opt/services/status/firewall_status.status
IPTABLES_STATUS=/opt/services/status/iptables_status.status

#����ǽ����Ϊ FW_FILTER
#����ǽSNAT����Ϊ   FW_SNAT
#����ǽDNAT����Ϊ	FW_DNAT
#�������Ƶ���Ϊ  TRAFFIC_CONTROL

#FORWARD���ϣ����� portal����ع���  ����ǽ�Ĺ���Ӧ�������
#����ǿ��start��������FORWARD�����һ��jump�� FW_FILTER �Ĺ��������
#stop���ǽ���������ȥ����

check_chain(){
	#���û��FW_FILTER��������ʹ��������
	iptables -L FW_FILTER > /dev/null 2>&1
	if [ ! $? -eq 0 ];then
		iptables -N FW_FILTER
		iptables -A FORWARD -j FW_FILTER
	else
		iptables -D FORWARD -j FW_FILTER
		iptables -A FORWARD -j FW_FILTER
	fi
	
	iptables -t nat -L FW_DNAT > /dev/null 2>&1
	if [ ! $? -eq 0 ];then
		iptables -t nat -N FW_DNAT
		iptables -t nat -A PREROUTING -j FW_DNAT
	else
		iptables -t nat -D PREROUTING -j FW_DNAT
		iptables -t nat -A PREROUTING -j FW_DNAT
	fi
		
	iptables -t nat -L FW_SNAT > /dev/null 2>&1
	if [ ! $? -eq 0 ];then
		iptables -t nat -N FW_SNAT
		iptables -t nat -A POSTROUTING -j FW_SNAT
	else
		iptables -t nat -D POSTROUTING -j FW_SNAT
		iptables -t nat -A POSTROUTING -j FW_SNAT
	fi

#traffic control ��ص���,���Ҫ����ǰ��,��Ϊ�ڽ�FW_FILTER��TRAFFIC_CONTROL�ŵ�FORWARD������,Ҫ��TRAFFIC_CONTROL����ǰ��
	iptables -t mangle -L TRAFFIC_CONTROL > /dev/null 2>&1
	if [ ! $? -eq 0 ];then
		iptables -t mangle -N TRAFFIC_CONTROL
		iptables -t mangle -A PREROUTING -j TRAFFIC_CONTROL
	else
		iptables -t mangle -D PREROUTING -j TRAFFIC_CONTROL
		iptables -t mangle -A PREROUTING -j TRAFFIC_CONTROL
	fi
	
	iptables -L TRAFFIC_CONTROL > /dev/null 2>&1
	if [ ! $? -eq 0 ];then
		iptables -N TRAFFIC_CONTROL
		iptables -A FW_FILTER -j TRAFFIC_CONTROL
	else
		iptables -D FW_FILTER -j TRAFFIC_CONTROL
		iptables -I FW_FILTER -j TRAFFIC_CONTROL
	fi
	
	#�ж�FW_FILER�������һ����ʲô����������ǽ����е�accept��������������һ������
	TEMP=$(iptables -nvxL FW_FILTER | tail -1 | awk -v FS=" " '{if($3=="ACCEPT"&&$4=="0"&&$5=="--"&&$6=="*"&&$7=="*"&&$8=="0.0.0.0/0"&&$9=="0.0.0.0/0"&& ! $10) print "TRUE"}')
	if [ "x$TEMP" != "xTRUE" ];then
		iptables -A FW_FILTER -j ACCEPT
	fi
	
	return 0
}


start() {
	echo "firewall load rules"
	/opt/services/init/iptables_init init
	iptables-restore ${FIREWALL_DATA}
	check_chain > /dev/null 2>&1
#���浱ǰϵͳ����ǽ��״̬
	echo "start" > ${FIREWALL_STATUS}
	
	return 0
}

stop() {
#FORWARD�н�jump��FW_FILTER����ȥ��
#	iptables -D FORWARD -j FW_FILTER > /dev/null 2>&1
#	iptables -t nat -D POSTROUTING -j FW_SNAT > /dev/null 2>&1
#	iptables -t nat -D PREROUTING -j FW_DNAT > /dev/null 2>&1
	check_chain > /dev/null 2>&1
	flush
	
	iptables -A FW_FILTER -j TRAFFIC_CONTROL
	iptables -A FW_FILTER -j ACCEPT
	iptables -t nat -A FW_SNAT -j ACCEPT
	iptables -A FW_INPUT -j RETURN
	iptables -t nat -A FW_DNAT -j ACCEPT
	iptables -t mangle -A PREROUTING -j TRAFFIC_CONTROL
#���浱ǰϵͳ����ǽ��״̬
	echo "stop" > ${FIREWALL_STATUS}
	
	return 0
}

restart() {
	stop
	start
	return 0
}

flush() {

	check_chain > /dev/null 2>&1
	iptables -F FW_FILTER
	iptables -t nat -F FW_SNAT
	iptables -t nat -F FW_DNAT
	#add by chensheng on 2010-04-08 for input chain
	iptables -F FW_INPUT
	
	return $?
}

save() {
#��Ϊ���ܱ���һ���������ݣ�����ֱ�ӱ�������iptables
#	/opt/services/init/iptables_init save
	iptables-save | sed "/.*CP_.*/d" | sed "/ipp2p/d" > ${FIREWALL_DATA}
	return $?
}

start_c() {
	check_chain > /dev/null 2>&1
#����c����д�ĳ�������ɹ����װ�أ�����ҳ���� �������رշ���ĵط�
	/opt/www/ccgi-bin/ws_fwdocommand.cgi 2>/dev/null
	if [ $? = '0' ]; then
		echo "start" > ${FIREWALL_STATUS}
	fi
}


case "$1" in
    start)
        stop
        start
        RETVAL=$?
        ;;
    stop)
    	echo "firewall stop"
        stop
        RETVAL=$?
        ;;
    restart)
        restart
        RETVAL=$?
        ;;
	flush)
		flush
		RETVAL=$?
		;;
	save)
		save
		RETVAL=$?
		;;                                
	check_chain)
		check_chain
		RETVAL=$?
		;;
	*)
        echo $"Usage: $0 {start|stop|restart|save|flush}"
        exit 1
        ;;
esac

exit $RETVAL

