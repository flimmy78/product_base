#! /bin/sh
### BEGIN INIT INFO
# Provides:          checkproductstate.sh
# Required-Start:    sem
# Required-Stop:     
# Should-Start:      
# Should-stop:
# Default-Start:     S
# Default-Stop:
# Short-Description: Check to sem is ok
### END INIT INFO
PRODUCTFILE=/dbm/product_state
check_product_state()
{
	printf "Checking PRODUCT state..."
	i=0;
	timepass=0 # time elapsed
	sleeptime=1
	while true 
	do
		if [ -s $PRODUCTFILE ] ; then
			product_state=`cat /dbm/product_state`
			if [ "$product_state" -gt 1 ] ; then
				printf " done. $timepass seconds waited.\n"
				return 0
			fi
			if [ "$i" -gt 99 ] ; then
				printf " done. time out.return 1\n"
				return 1
			fi
		fi

		i=$(($i+1));
		
		timepass=$i
		
		case $(($i%4)) in
			0) printf "-";;
			1) printf "\\";;
			2) printf "|";;
			3) printf "/";;
		esac
		printf " $(($timepass))s"
		# sleep for a while
		sleep $sleeptime
		# clear the process line
		printf "\b\b\b\b"
		if [ $timepass -gt 99 ] ; then
			printf "\b\b"
			printf " done. time out. return 2\n"
			return 2
		elif [ $timepass -gt 9 ] ; then
			printf "\b"
		fi

	done
}

check_product_state

