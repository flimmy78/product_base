#!/bin/sh
CheckProcess()
{
	PROCESS_NUM=`ps -ef|grep "$1"|grep -v grep|wc -l`
	if [ $PROCESS_NUM -eq 2 ]  || [ $PROCESS_NUM -eq 1 ] ; then
		return 0
	else
		return 1
	fi
}

while [ 1 ]; do
	CheckProcess "cli_tunnel_serv"
	CheckQQ_RET=$?
        ACTIVE_MASTER=`cat /dbm/local_board/is_active_master`
        MASTER=`cat /dbm/local_board/is_master`
	if [ $CheckQQ_RET -eq 1 ] ; then
		pkill cli_tunnel_serv
		cd /home/admin
		if [ $ACTIVE_MASTER -eq 1 ]  && [ $MASTER -eq 1 ] ; then
		    /usr/bin/cli_tunnel_server  /bin/login &
		    flag=1
		elif [ $ACTIVE_MASTER -eq 0 ]  && [ $MASTER -eq 1 ] ; then
		   su admin --shell  /usr/bin/cli_tunnel_server  /opt/bin/vtysh &
		    flag=0
		else
		   su admin --shell /usr/bin/cli_tunnel_server /opt/bin/vtysh &
		fi


         else
                 if [ $ACTIVE_MASTER -eq 0 ]  && [ $MASTER -eq 1 ] ; then
	               if [ $flag -eq 1 ] ; then 
	                 pkill cli_tunnel_serv
			 cd /home/admin
			 su admin --shell /usr/bin/cli_tunnel_server  /opt/bin/vtysh &
			 flag=0
		       fi
		 fi
                 if [ $ACTIVE_MASTER -eq 1 ]  && [ $MASTER -eq 1 ] ; then
                       if [ $flag -eq  0 ] ; then
		          pkill  cli_tunnel_serv
			  cd /home/admin
                          /usr/bin/cli_tunnel_server  /bin/login  &
			  flag=1
		       fi
		 fi
         fi
	sleep 5
done
