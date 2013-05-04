#!/bin/bash
# install script for packages

if [ ! $# -eq 1 ] ; then
	echo "usage: install.sh PKGNAME"
	exit 1;
fi
PKGNAME=$1 
./opfiles.sh $PKGNAME/filelist $PKGNAME/files ../files CP 
