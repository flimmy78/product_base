#!/bin/bash

module="bm"

device="bm"

mode="664"

major=236

# invoke insmod with all arguments we got

# and use a pathname, as newer modutils don't look in . by default

/sbin/insmod $module.ko debug_ioctl=1 $* || exit 1

# remove stale nodes

rm -f /dev/${device}[0-3]


#using mixed quote is messy
major=$(awk '$2=="'$module'" {print $1}' /proc/devices)

#using variable is more readable
#major=$(awk -v mod="$module" '$2==mod {print $1}' /proc/devices)

echo "major is "${major}"."

mknod /dev/${device}0 c ${major} 0

