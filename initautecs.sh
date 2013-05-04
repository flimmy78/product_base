#!/bin/bash

./initpublic.sh

pushd src
[ -d AuteCS ] || cvs checkout AuteCS
[ -d checkpasswd-pam ] || cvs checkout checkpasswd-pam
[ -d eag ] || cvs checkout eag 
pushd checkpasswd-pam
./configpkg-mips.sh
popd
popd
