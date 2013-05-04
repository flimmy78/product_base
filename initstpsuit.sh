#!/bin/bash

./initpublic.sh

pushd src
[ -d stpsuit ] || cvs checkout stpsuit
popd
