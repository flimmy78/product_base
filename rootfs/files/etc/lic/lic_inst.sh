#!/bin/bash

if [ ! $# -eq 1 ] ; then
echo "Usage: lic_inst.sh LIC_ENCRYPT"
exit 1;
fi

LIC_ENCRYPT=$1

LIC_MAXWTPCOUNT_VALUE=`/etc/lic/lic_getvalue.sh $LIC_ENCRYPT`
RET=$?
if [ $RET -eq 0 ] ; then
#default license type is empty, used 0/1 two values.
LICTYPE=''
elif [ $RET -eq 65 ] ; then
LICTYPE=2
elif [ $RET -eq 66 ] ; then
LICTYPE=3
elif [ $RET -eq 67 ] ; then
LICTYPE=4
elif [ $RET -eq 68 ] ; then
LICTYPE=5
elif [ $RET -eq 69 ] ; then
LICTYPE=6
elif [ $RET -eq 70 ] ; then
LICTYPE=7
elif [ $RET -eq 71 ] ; then
LICTYPE=8
elif [ $RET -eq 72 ] ; then
LICTYPE=9
else
exit $RET
fi


[ -d /mnt/lic ] || mkdir /mnt/lic
printf ${LIC_ENCRYPT} > /mnt/lic/newtmplic${LICTYPE}
mv --backup=t /mnt/lic/newtmplic${LICTYPE} /mnt/lic/maxwtpcount${LICTYPE}.lic
sor.sh cp lic/maxwtpcount${LICTYPE}.lic 30

printf ${LIC_MAXWTPCOUNT_VALUE} > /devinfo/maxwtpcount${LICTYPE}
sudo chmod 777 -R /mnt/lic
exit 0
