#!/bin/bash

./initdirs.sh

pushd src
[ -d accapi ] || cvs checkout accapi
[ -d dcli ] || cvs checkout dcli
[ -d bashtools ] || cvs checkout bashtools
popd


