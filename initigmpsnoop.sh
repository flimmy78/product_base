#!/bin/bash

./initpublic.sh

pushd src
[ -d igmp-snooping ] || cvs checkout igmp-snooping
popd
