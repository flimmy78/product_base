#!/bin/bash
rootdir=`pwd`

if [ -n "$2" ] ; then
    pb_dir=$2
else
    pb_dir="$rootdir/src/accapi_pb"
fi

daem_dir="$rootdir/src/dg/accapi"
if [ "$1" = "copy" ] ; then
    action0="rm -f"
    action=cp
    action_desc="Copying"
elif [ "$1" = "diff" ] ; then
    action0=:
    action="diff -u"
    action_desc="Diffing"
elif [ "$1" = "link" ] ; then
    action0="rm -f"
    action="ln -s -f"
    action_desc="Linking"
else
    echo "Usage: link-header.sh [copy | diff | link] {pb_dir}"
    exit 1
fi

echo "$action_desc from $pb_dir to $daem_dir"

(
cd $daem_dir
echo "Working in $daem_dir"

{ while read file ; do
	if [ -d $daem_dir/$file ] ; then
	$action0 $file
	$action $pb_dir/$file .
	echo "$action $pb_dir/$file $daem_dir/$file"
	else
	$action $pb_dir/$file .
	fi
done } <<EOF
board
config
nam
nbm
sem
npd
EOF
)
