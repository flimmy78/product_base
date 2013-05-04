#!/bin/bash

#TODO add test whether environment variable and symbol link already exist.


[ -d export ] || mkdir export
[ -d export/lib ] || mkdir export/lib
NPD_ROOT_PATH=$(pwd)

ln -s ${NPD_ROOT_PATH}/src/include export/include 
ln -s ${NPD_ROOT_PATH}/src/res export/res 
line0='NPD_ROOT_PATH='$(pwd)' '
linea='export NPD_ROOT_PATH'
line1='NPD_EXPORT_INCLUDE=-I'$(pwd)'/export/include'
line2='export NPD_EXPORT_INCLUDE'

echo $line0
echo $linea
echo $line1
echo $line2


cat <<EOF >> ~/.bashrc
${line0}
${linea}
${line1}
${line2}
EOF

