#!/bin/bash

./initpublic.sh

pushd src
[ -d wcpss ] || cvs checkout wcpss
popd
