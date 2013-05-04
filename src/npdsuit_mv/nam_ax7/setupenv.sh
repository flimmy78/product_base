#!/bin/bash

#TODO add test whether environment variable and symbol link already exist.


[ -d export ] || mkdir export
[ -d export/lib ] || mkdir export/lib
NAM_ROOT_PATH=$(pwd)

ln -s ${NAM_ROOT_PATH}/src/include export/include 
ln -s ${NAM_ROOT_PATH}/src/res export/res 

line0='NAM_ROOT_PATH='$(pwd)' '
linea='export NAM_ROOT_PATH'
line1='NAM_EXPORT_INCLUDE=-I'$(pwd)'/export/include'
line2='export NAM_EXPORT_INCLUDE'
line3='NAM_EXPORT_LIB=-L'$(pwd)'/export/lib'
line4='export NAM_EXPORT_LIB'
line5='NAM_EXPORT_LIBS="'-lnam' "'
line6='export NAM_EXPORT_LIBS'

echo $line0
echo $linea
echo $line1
echo $line2
echo $line3
echo $line4
echo $line5
echo $line6


cat <<EOF >> ~/.bashrc
${line0}
${linea}
${line1}
${line2}
${line3}
${line4}
${line5}
${line6}
EOF

