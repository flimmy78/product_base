#!/bin/bash
if [ $# -gt 0 ]; then
IMGNAME=$1
else
IMGNAME=AW
fi

cd release

compress_bin()
{
echo "Convert the kernel into a raw binary image ..."
mips64-octeon-linux-gnu-objcopy -O binary -R .note -R .comment -S vmlinux linux.bin
echo "Compressing the kernel bin..."
gzip -9 linux.bin
echo "Packaging compressed image for U-boot"
../mkimage -A mips -O linux -T kernel -C gzip -a 21000000 -n $IMGNAME -d linux.bin.gz AW.IMG
}

compress_elf()
{
echo "Compressing the kernel elf..."
gzip -9 vmlinux
tar  -cvvjf auteware.tar.bz2 auteware
echo "Packaging compressed image for U-boot"
#../mkimage -A mips -O linux -T kernel -C gzip -a 9000000 -n $IMGNAME -d vmlinux.gz AW.IMG
../mkimage -A mips -O linux -T multi -C gzip -a 21000000 -n $IMGNAME -d vmlinux.gz:auteware.tar.bz2 AW.IMG
}

# uncompress and load bin is not supported by boot by now
compress_elf

echo "Verifying image information ... "
../mkimage -l AW.IMG
cd ..
