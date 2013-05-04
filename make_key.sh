#!/bin/bash
if [ $# != 1 ] ; then
	echo "press only one parameter"
	exit 0
fi
temp_str=$1
echo $temp_str > passwd

openssl genrsa -passout file:passwd -des3 4096 > pri.pem

openssl rsa -in pri.pem -pubout -out pub.pem -passin file:passwd

cp pub.pem auteware/files/etc/ -f
