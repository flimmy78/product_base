#!/bin/bash

./initpublic.sh
pushd src

[ -d quagga ] || cvs checkout quagga
pushd quagga
./configpkg
popd


popd
