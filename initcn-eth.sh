#!/bin/bash

./initpublic.sh
pushd src
cvs checkout cavium-ethernet 
popd

