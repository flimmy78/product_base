#! /bin/sh
### BEGIN INIT INFO
# Provides:          SW 
# Required-Start:
# Required-Stop:
# Should-Start:      setaffi
# Default-Start:     S
# Default-Stop:
# Short-Description: Set affinity of init and interrupts.
# Description:       Set affinity of init to 0x1 so that each process created by it will automatically 
#			inherits 1 by default.
#			Any process wishes to have special affinity will have to be set manually.
#			affinity of each interrupt will have to be set one by one.
#			Set to start after bootlog
### END INIT INFO

PATH=/sbin:/bin:/usr/bin:/usr/sbin

. /lib/init/vars.sh
. /lib/lsb/init-functions

product_id=`cat /proc/product_id`


set_one_process_affinity()
{
  pid=$1
  log_action_msg " "`taskset -p 1 $pid 2>&1`
}

set_root_process_affinity()
{
	log_action_msg "Setting smp affinity of initial processes .."

#init process
	set_one_process_affinity 1

#udevd process
	pidofudevd=`pidof udevd`
	set_one_process_affinity $pidofudevd
}

set_three_irq_affinity() {
	log_action_begin_msg "$1 $2 to e"
	if [ -e /proc/irq/$2/smp_affinity ] ; then
	    echo e > /proc/irq/$2/smp_affinity
	    log_action_end_msg $?
	else
	    log_failure_msg " doesn't exist."
	fi
}
set_one_irq_affinity() {
	log_action_begin_msg " $1 $2 to 1" 
	if [ -e /proc/irq/$2/smp_affinity ] ; then
	    echo 1 > /proc/irq/$2/smp_affinity
	    log_action_end_msg $? 
	else
	    log_failure_msg " doesn't exist."
	fi
}

set_all_irq_affinity()
{
	log_action_msg "Setting smp affinity of irqs .."

#42 for serial in X5
#44,for marvell pp 275 in X7 , for 263 in X5
#45, for marvell pp 804
#46 for cardbus
#54 for spi/rgmii management irq
#	set affinity 42 will cause kernel exception on 3840 board
      set_one_irq_affinity "pci irq" 44
      set_one_irq_affinity "pci irq" 45
      set_one_irq_affinity "pci irq" 46
	# C5A000 for Chassis 5 slots Cheetah2
	if [ $(($product_id & 0xFFF000 )) -eq $((0xC5A000)) ]; then
      set_one_irq_affinity "pci irq" 54
	# B1A000 for Box product 1 slot, cheetah2
	elif [ $(($product_id & 0xFFF000 )) -eq $((0xB1A000)) ]; then 
      set_one_irq_affinity "serial irq" 42
	# if there is usb flash, then set usb interrupt only work on one core
      set_one_irq_affinity "usb irq" 64
	elif [ $(($product_id & 0xFFFFFF )) -eq $((0xB1C001)) ]; then
      set_three_irq_affinity "cavium ethernet" 23
      set_three_irq_affinity "pcie msi" 48
      set_three_irq_affinity "82571 eth1-5" 160
      set_three_irq_affinity "82571 eth1-6" 161
      set_three_irq_affinity "82571 eth1-7" 162
      set_three_irq_affinity "82571 eth1-8" 163
	elif [ $(($product_id & 0xFFFFFF )) -eq $((0xB1C101)) ]; then
      set_three_irq_affinity "pcie msi" 48
      set_three_irq_affinity "82571 eth1-1" 160
      set_three_irq_affinity "82571 eth1-2" 161
      set_three_irq_affinity "82571 eth1-3" 162
      set_three_irq_affinity "82571 eth1-4" 163
        fi

}

do_dataplane_affinity() {
#23 for pow_receive irq which will be used to receive packets
#This doesn't work, recv tasklet will check whether it's core 0
	coremask=`cat /proc/coremask`
	if [ $(($coremask)) -gt 1 ]; then
		newmask=$(($coremask & 0xfe))
		if [ $(($newmask)) -gt 1 ]; then
			echo $newmask > /proc/irq/23/smp_affinity
			printf "Set smp affinity of irq 23 to %#04x\n" $newmask
		fi
	fi
}

do_start () {

#	If didnot set irq affinity, kap0 will be created after kap module was loaded, which will cause failure in loadmodule. 
# Try to only set loadmodule's affinity to 1instead of init.
# Let many app use multi-core is still dangerous.
	set_root_process_affinity


# Many drivers doesn't support multi-core, so set irq affinity is necessary.
	set_all_irq_affinity

# Suppose to work on cpu with more than 1 core	
#	do_dataplane_affinity 

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
	# No-op
	;;
  *)
	echo "Usage: setaffinity.sh [start|stop]" >&2
	exit 3
	;;
esac
