#!/bin/bash

./initpublic.sh

pushd src
[ -d asd ] || cvs checkout asd
popd
