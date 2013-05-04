#!/bin/bash

make -C ../src/app
make -C ../src/kmod
cp *mod.sh /opt/CN/rootfs/files/opt/
cp op_mv82210.sh /opt/CN/rootfs/files/opt/
cp ../exec/bm* /opt/CN/rootfs/files/opt/
ls -l /opt/CN/rootfs/files/opt/
