#!/bin/bash

if [ ! $# -eq 1 ] ; then
echo "Usage: lic_getvalue.sh LIC_ENCRYPT"
exit 1;
fi

LIC_ENCRYPT=$1

#LIC_CLEAR=`echo $LIC_ENCRYPT | openssl base64 -A -d | openssl rsautl -verify -pubin -inkey /etc/keys/ac_key.pem `
LIC_CLEAR_ORIG=`echo $LIC_ENCRYPT | openssl base64 -A -d | openssl rsautl -verify -pubin -inkey /etc/lic/keys/ac_key.pem 2>/dev/null`
LIC_CLEAR=`echo ${LIC_CLEAR_ORIG} | sed -e 's/\r//g' -e 's/^[[:blank:]]*//g' -e 's/[ \t]*$//' -e '/^$/d'`
if [ ${#LIC_CLEAR} -le 42 ] ; then
	echo "Failed to verify license."
	exit 2;
fi

LICREQ_DEV=`/etc/lic/licreq_gen.sh`
LICREQ_NEW=${LIC_CLEAR:0:40}
if [ ! ${LICREQ_NEW} = ${LICREQ_DEV} ] ; then
	LICREQ_DEV=`/etc/lic/licreq_gen2.sh`
	if [ ! ${LICREQ_NEW} = ${LICREQ_DEV} ] ; then
		echo -e "License request info from license \n$LICREQ_NEW\ndoesn't match this device \n$LICREQ_DEV\nPlease contact vendor."
		exit 3;
	fi
fi


LIC_VALUE_LEN=$((${#LIC_CLEAR}-42))
LIC_MAXWTPCOUNT_VALUE=${LIC_CLEAR:42:${LIC_VALUE_LEN}}
if [ ${LIC_MAXWTPCOUNT_VALUE} -gt 40960 ] ; then
	echo "Unsupported license value $LIC_MAXWTPCOUNT_VALUE.";
	exit 5;
fi

#
#It's ok to downgrade license.
#
#CUR_MAXWTPCOUNT=`cat /devinfo/maxwtpcount 2>/dev/null`
#if [ ${LIC_MAXWTPCOUNT_VALUE} -le ${CUR_MAXWTPCOUNT:-128} ] ; then
#	echo "License maxwtpcount ${LIC_MAXWTPCOUNT_VALUE} is less or equal to current maxwtpcount ${CUR_MAXWTPCOUNT}, new license will be ignored."
#	exit 6;
#fi

LIC_TYPE_MAXWTPCOUNT=01
LIC_TYPE_MAXWTPCOUNT2=02
LIC_TYPE_MAXWTPCOUNT3=03
LIC_TYPE_MAXWTPCOUNT4=04
LIC_TYPE_MAXWTPCOUNT5=05
LIC_TYPE_MAXWTPCOUNT6=06
LIC_TYPE=${LIC_CLEAR:40:2}
if [  x${LIC_TYPE} = x${LIC_TYPE_MAXWTPCOUNT} ] ; then
	sudo echo ${LICREQ_NEW} > /devinfo/licreq
	printf ${LIC_MAXWTPCOUNT_VALUE}
	exit 0;
elif [  x${LIC_TYPE} = x${LIC_TYPE_MAXWTPCOUNT2} ] ; then
	sudo echo ${LICREQ_NEW} > /devinfo/licreq2
	printf ${LIC_MAXWTPCOUNT_VALUE}
	exit 65;
elif [  x${LIC_TYPE} = x${LIC_TYPE_MAXWTPCOUNT3} ] ; then
	sudo echo ${LICREQ_NEW} > /devinfo/licreq3
	printf ${LIC_MAXWTPCOUNT_VALUE}
	exit 66;
elif [  x${LIC_TYPE} = x${LIC_TYPE_MAXWTPCOUNT4} ] ; then
	sudo echo ${LICREQ_NEW} > /devinfo/licreq4
	printf ${LIC_MAXWTPCOUNT_VALUE}
	exit 67;
elif [  x${LIC_TYPE} = x${LIC_TYPE_MAXWTPCOUNT5} ] ; then
	sudo echo ${LICREQ_NEW} > /devinfo/licreq5
	printf ${LIC_MAXWTPCOUNT_VALUE}
	exit 68;
elif [  x${LIC_TYPE} = x${LIC_TYPE_MAXWTPCOUNT6} ] ; then
	sudo echo ${LICREQ_NEW} > /devinfo/licreq6
	printf ${LIC_MAXWTPCOUNT_VALUE}
	exit 69;
else
	echo "Unsupported license type ${LIC_TYPE}."
	exit 4;
fi 
