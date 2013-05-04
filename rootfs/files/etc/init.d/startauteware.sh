#! /bin/sh


AUTEWAREFILE="/mnt/auteware"
BOOTIMGNAME=`cat /var/run/bootimgname`

decomp_auteware() {
	
	getauteware $1
#	mkdir $AUTEWAREFILE
	rm $1
	pushd /mnt
	tar xf /mnt/auteware.tar.bz2 1> /var/log/bootimg  2>&1 
	rm auteware.tar.bz2
	popd  
}

copy_auteware() {
	cp -rf $1/files/etc/init.d/* /etc/init.d/
	cp -rf $1/files/etc/rc2.d/* /etc/rc2.d/
	cp -rf $1/files/etc/motd /etc/motd
	cp -rf $1/files/opt/* /opt/
	cp -rf $1/files/usr/bin/* /usr/bin
	cp -rf $1/files/etc/pub.pem /etc 2> /dev/null
	rm -rf $1
}


decomp_auteware "/mnt/""$BOOTIMGNAME"
copy_auteware $AUTEWAREFILE

chmod u+s /opt/bin/vtysh

check_mod()
{
if [ -e $1 ] ; then
        chmod u+s $1
fi
}

check_mod /opt/bin/iptables
check_mod /opt/bin/iptables-save
check_mod /opt/bin/iptables-restore
check_mod /opt/bin/ipset
check_mod /opt/bin/autelan-subagent
check_mod /opt/bin/trap-helper
check_mod /usr/sbin/tcpdump
check_mod /sbin/tc


