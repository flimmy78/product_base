#!/bin/bash

SN=`cat /proc/sysinfo/product_sn 2>/dev/null`
if [ ! ${#SN} -eq 20 ] ; then
	SN=EEEEEEEEEEEEEEEEEEEE
fi
 
MAC=`cat /proc/sysinfo/product_base_mac_addr 2>/dev/null`
if [ ! ${#MAC} -eq 12 ] ; then
	MAC=abcdefghijkl
fi

PREFIX=ABCD
TAIL=5678
LICREQ=$PREFIX$SN$MAC$TAIL

echo $LICREQ | tr [:lower:] [:upper:]
