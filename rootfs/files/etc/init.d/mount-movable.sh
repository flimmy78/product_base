#! /bin/sh
### BEGIN INIT INFO
# Provides:          mount-movable
# Required-Start:    
# Required-Stop: 
# Should-Start:      
# Should-Stop:
# Default-Start:     S
# Default-Stop:
# Short-Description: Mount all movable devices
# Description:
### END INIT INFO

PATH=/sbin:/bin:/usr/bin:/usr/sbin:/opt/bin
. /lib/init/vars.sh

. /lib/lsb/init-functions
. /lib/init/mount-functions.sh
. /etc/init.d/loaddevinfo
#. /usr/bin/libinstpatch.sh

mounted=0

log_maxsize_sd=$((64*1024*1024))
log_maxsize_flash=$((8*1024*1024))
log_maxsize=$log_maxsize_flash

FSCK_LOGFILE=/var/log/checkfs.log

mountpoint=/blk

mount_all_movable() {
	loop=1
	while true
	do
	{
		if [ -e /dev/mmcblk0p1 ]; then
			echo "/dev/mmcblk0p1	$mountpoint		vfat	umask=000	0	0" >> /etc/fstab
	 		mount $mountpoint 
			log_maxsize=$log_maxsize_sd
			mounted=1
			break;
		elif [ -e /dev/sda1 ]; then
			echo "/dev/sda1	$mountpoint		vfat	umask=000	0	0" >> /etc/fstab
			mount $mountpoint 
			log_maxsize=$log_maxsize_sd
			mounted=1
			break;
		elif [ -e /dev/sda ]; then
			echo "/dev/sda	$mountpoint		vfat	umask=000	0	0" >> /etc/fstab
			mount $mountpoint
			log_maxsize=$log_maxsize_sd
			mounted=1
			break
		elif [ -e /dev/cfa1 ]; then
			echo "/dev/cfa1 $mountpoint		vfat	umask=000	0 	0" >> /etc/fstab
			mount $mountpoint
			mounted=1
			break
		elif [ -e /dev/cfa ]; then
			echo "/dev/cfa	$mountpoint		vfat	umask=000	0	0" >> /etc/fstab
			mount $mountpoint
			mounted=1
			break
		else
		printf "."
		sleep 1
		fi
		loop=$(($loop+1))
		if [ $loop -gt 20 ] ; then
			echo -e "\nFatal Error: No storage media found, please contact vendor."
			echo -e "\nFatal Error: No storage media found during startup." >> /etc/motd
			while true 
			do
				read cmd
				if [ "$cmd" = "out" ] ; then
					break;
				fi
			done
			break;
		fi
	}	
	done
}
check_fs()
{
    if [ -e /dev/mmcblk0p1 ]; then
        fsck.vfat -v -a /dev/mmcblk0p1 > $FSCK_LOGFILE
        break;
    elif [ -e /dev/cfa1 ]; then
        fsck.vfat -v -a /dev/cfa1 > $FSCK_LOGFILE
        break;
    elif [ -e /dev/cfa ]; then
        fsck.vfat -v -a /dev/cfa > $FSCK_LOGFILE
        break;
    elif [ -e /dev/sda1 ]; then
        fsck.vfat -v -a /dev/sda1 > $FSCK_LOGFILE
        break;
    elif [ -e /dev/sda ]; then
        fsck.vfat -v -a /dev/sda > $FSCK_LOGFILE
        break;
    else
        printf "No Storage Device\n"
    fi

}

pre_mountall()
{
	[ -d /blk ] || mkdir /blk
	[ -d /mnt ] || mkdir /mnt
	[ -d /ext ] || mkdir /ext
	[ -d /mnt/wtp ] || mkdir /mnt/wtp
	[ -d /mnt/rtsuit ] || mkdir /mnt/rtsuit
	[ -d /mnt/patch ] || mkdir /mnt/patch
	[ -d /mnt/logarchives ] || mkdir /mnt/logarchives
	[ -d /mnt/critlog ] || mkdir /mnt/critlog

	chmod 777 /blk
	chmod 777 /mnt
	chmod 777 /mnt/wtp
	chmod 777 /mnt/rtsuit
	chmod 777 /mnt/patch
	chmod 777 /mnt/logarchives
	chmod 777 /mnt/critlog
	> /etc/motd

	if [ -e /dev/sda2 ]; then 
		echo "/dev/sda2	/ext	 	ext2	defaults	0	0" >> /etc/fstab
		mount /ext
	fi

	#Check and repair the FAT file system in each time the system is started. Jia Lihui.

	echo "Skip check_fs !!!"
	#check_fs 
}

load_one_file()
{
#	if [ ! -e /blk/$2 ] ; then
#		return 0;
#	fi
	ls /blk/$2 > /dev/null 2>&1
	if [ ! $? -eq 0 ] ; then
		return 0;
	fi

	log_action_begin_msg " $1 "
	destdir=`dirname /mnt/$2`
	[ -d $destdir ] || mkdir -p $destdir ; chmod 777 $destdir
	cp -rf /blk/$2 $destdir
	chmod a+rw /mnt/$2
	ret=$?
	log_action_end_msg $ret
	if [ ! $ret -eq 0 ] ; then
		echo "Failed load $1 from storage media during startup." >> /etc/motd
	fi
}

load_var_run_config()
{
	ls /blk/$2 > /dev/null 2>&1
	if [ ! $? -eq 0 ] ; then
		return 0;
	fi
	
	[ -d /var ] || mkdir /var
	[ -d /var/run ] || mkdir /var/run
	[ -d /blk/config ] || mkdir /blk/config
	
	chmod 777 /var
	chmod 777 /var/run
	chmod 777 /blk/config
	
	log_action_begin_msg " $1 "
	destdir=`dirname /var/run/$2`
	[ -d $destdir ] || mkdir -p $destdir ; chmod 777 $destdir
	cp -rf /blk/$2 $destdir
	chmod a+rw /var/run/$2
	ret=$?
	log_action_end_msg $ret
	if [ ! $ret -eq 0 ] ; then
		echo "Failed load $1 from storage media during startup." >> /etc/motd
	fi
}


load_case_insensitive_files()
{
pat=$1
find /blk/wtp/ -iname "*${pat}"  -fprint /var/run/wtp${pat}list 
for imgfile in `cat /var/run/wtp${pat}list`
do
	baseimgname=`basename $imgfile`
	log_action_begin_msg " $baseimgname "
	cp -rf $imgfile /mnt/wtp/$baseimgname
	chmod a+rw /mnt/wtp/$baseimgname
	ret=$?
	log_action_end_msg $ret
	if [ ! $ret -eq 0 ] ; then
		echo "Failed load $baseimgname from storage media during startup." >> /etc/motd
	fi
done
}

SYSOPLOG="critlog/sysop.log"
load_syslog_file()
{
if [ -f /blk/syslog.tar.bz2 ] ; then
	log_action_begin_msg " Syslog Files "
	tar xjPf /blk/syslog.tar.bz2 -C /
	ret=$?
	log_action_end_msg $ret
fi
[ -f /var/log/syslogservice.log ] || echo "Syslog service." > /var/log/syslogservice.log
}
load_sys_boot_img()
{
	getbootimg > /var/run/bootimgname
	if [ $? -nt 0 ] ; then

	echo "run getbootimg error"
	return -1

	fi

	
	BOOTIMGNAME=`cat /var/run/bootimgname`
	echo $BOOTIMGNAME
	load_one_file "Load bootimg" $BOOTIMGNAME
}

load_blk_files()
{
	load_one_file "Configuration" *.conf 
#	load_one_file "Rtsuit" rtsuit 
#only copy the ".conf" file not include ".conf~" .
	load_one_file "Rtsuit" rtsuit/*.conf 
	load_one_file "Device Info" devinfo 
	load_one_file "Show Code" showcode 
	load_one_file "Force Convert String" forcevstring 
	load_one_file "Eearly Startup" earlystart 
	load_one_file "Start" startup 
	load_one_file "Logo" logo/*
	load_one_file "License" lic/* 
	load_one_file "Patches" patch/*
	load_one_file "Embedded-Portal Customization Pages" eag_html/*
	load_one_file "WTP Compatible" wtp/wtpcompatible.xml 
	load_one_file "SYSOP Record" $SYSOPLOG
	load_one_file "NEW_CONFIG" new_config/*.conf
    	load_one_file "NEW_CONFIG_FLAG" new_config_flag	
	load_var_run_config "CONFIG" config/*
	
	load_sys_boot_img

	load_case_insensitive_files cer
	load_case_insensitive_files pem
	load_case_insensitive_files p12
	load_case_insensitive_files img
	load_case_insensitive_files bin
	#for netgear version file.
	load_case_insensitive_files tar
	load_syslog_file
}

convert_filename_to_lowcase()
{
dir1=$1
pushd $dir1
for file1 in `ls $dir1`
do
	mv $file1 `echo $file1 | tr "[A-Z]" "[a-z]"`

done
popd



}

check_softreboot()
{
log_action_msg "Checking system boot type"
if [ -f /blk/softreboot ] ; then
SRFLAG=`cat /blk/softreboot`
else
SRFLAG=0
fi

if [ x"$SRFLAG" = x"1" ] ; then
  echo 1 > /var/run/softreboot
  log_action_msg " Software Manual Reboot"
elif [ x"$SRFLAG" = x"2" ] ; then
  echo 2 > /var/run/softreboot
  log_action_msg " Software BaseGuard Reboot"
else
  echo 0 > /var/run/softreboot
  log_action_msg " NOT Software Reboot"
fi

echo 0 > /mnt/softreboot
chmod 666 /mnt/softreboot
echo 0 > /blk/softreboot
sync

}

check_softupgrade()
{
log_action_msg "Checking software upgrade state ..."
curosver=`cat /etc/version/verstring`
log_action_msg " Current OS Version $curosver"
if [ -f /blk/runosver ] ; then
	preosver=`cat /blk/runosver`
	log_action_msg " Preivous-run OS Version $preosver"
	if [ x"$preosver" = x"$curosver" ] ; then
		echo 0 > /mnt/softupgraded
	else
		log_action_msg " OS upgraded/downgraded successfully."
		echo 1 > /mnt/softupgraded
		echo $curosver > /blk/runosver
	fi
else
	echo 0 > /mnt/softupgraded
	echo $curosver > /blk/runosver
fi
sync
}
check_patch()
{
if [ -d /blk/patch ] ; then
	echo " Patch dir exist ."
else
	echo " Patch dir not exist ."
	mkdir /blk/patch	
fi
sync
}

check_newconf()
{
log_action_msg "Checking configuration upgrade state ..."
if [ -f /blk/newconf ] ; then
NEWCONF=`cat /blk/newconf`
else
NEWCONF=0
fi

if [ x"$NEWCONF" = x"1" ] ; then
echo 1 > /var/run/newconf
log_action_msg "New config was loaded ..."
else
echo 0 > /var/run/newconf
log_action_msg "No New config was loaded ..."
fi

echo 0 > /blk/newconf
sync

}




post_mountall()
{
	if [ $mountpoint = "/blk" ] ; then
	    if [ $mounted -eq 1 ] ; then
		log_action_msg "Loading files from storage media to memory"

#FAT doesn't support filename case change.
#		convert_filename_to_lowcase /blk
#		convert_filename_to_lowcase /blk/wtp
#		convert_filename_to_lowcase /blk/lic
#		convert_filename_to_lowcase /blk/logo
#		convert_filename_to_lowcase /blk/eag_html
#		sync
		check_patch
		load_blk_files

# This func defined in /etc/init.d/loaddevinfo
		load_devinfo

		check_softreboot

# Need devinfo generate verstring to check software oem version.
		check_softupgrade

# Check whether newconf flag was set by download conf cmd.
		check_newconf

		echo 1 > /var/log/archive

		umount /blk

		log_action_begin_msg "Starting storage agent daemon"
		start-stop-daemon -b -m -p /var/run/sad.sh.pid --start --exec /usr/bin/sad.sh
#		start-stop-daemon --start --exec /usr/bin/sad.sh &
		log_action_end_msg $?
	    fi
	fi
         
	echo $log_maxsize > /var/log/log_maxsize

        if [ -d /mnt/earlystart ] ; then
		printf "Running earlystart scripts ...\n"
		run-parts /mnt/earlystart
		#run-parts /mnt/startup --verbose 
		printf "Done.\n"
        fi



}

do_start() {
	#
	# 
	#
	pre_mountall
	log_action_begin_msg "Mounting storage device"
	mount_all_movable
	log_action_end_msg $?
	post_mountall
}

do_stop() {
	#
	# 
	#
	umount_all_movable() {
	    umount /blk
	}
	log_action_begin_msg "Umounting storage device"
	umount_all_movable
	log_action_end_msg $?
}

case "$1" in
  start|"")
	do_start
	;;
  restart|reload|force-reload)
	echo "Error: argument '$1' not supported" >&2
	exit 3
	;;
  stop)
	do_stop
	;;
  *)
	echo "Usage: mount-movable.sh [start|stop]" >&2
	exit 3
	;;
esac

exit 0

