#!/bin/bash

if [ ! $# -eq 4 ] ; then
	echo "usage: opfiles FILELIST SRCDIR DSTDIR [MV|CP|RM]"
	exit 1;
fi
FILELIST=$1
SRCDIR=$2
DSTDIR=$3
if [ $4 = "MV" ] ; then
OP=mv;
elif [ $4 = "CP" ] ; then
OP='cp -P';
else
OP='rm';
fi


#cat $FILELIST
while read -r line ; 
do
if [ "$OP" = "rm" ] ; then
	if [ ! -d $DSTDIR/$line ] ; then
		rm $DSTDIR/$line ;
	fi
else
#	echo [$SRCDIR][$line]
	if [ -d $SRCDIR/$line ] ; then
		mkdir -p "$DSTDIR/$line";
	else
		DIRNAME=`dirname $DSTDIR/$line`
		[ -d $DIRNAME ] || mkdir -p $DIRNAME
		$OP "$SRCDIR/$line" "$DSTDIR/$line"
	fi
fi
done < $FILELIST 
