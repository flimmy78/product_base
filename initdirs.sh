#!/bin/bash

[ -d src ] || mkdir src
[ -d export ] || mkdir export
[ -d export/opt ] || mkdir export/opt
[ -d export/opt/bin ] || mkdir export/opt/bin
[ -d export/opt/lib ] || mkdir export/opt/lib
[ -d export/opt/lib/iptables ] || mkdir export/opt/lib/iptables
[ -d export/opt/lib/ipset ] || mkdir export/opt/lib/ipset
#[ -d export/opt/etc ] || mkdir export/opt/etc
[ -d export/opt/awk ] || mkdir export/opt/awk
[ -d export/opt/www ] || mkdir export/opt/www
[ -d export/opt/www/ccgi-bin ] || mkdir export/opt/www/ccgi-bin
[ -d export/kmod ] || mkdir export/kmod
#[ -d export/ipt ] || mkdir export/ipt
#[ -d export/ipset ] || mkdir export/ipset


