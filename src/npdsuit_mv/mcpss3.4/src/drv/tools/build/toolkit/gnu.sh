# toolkit defs

export TOOL_FAMILY=gnu

export XCOMP_INC_PATH=/
export XCOMP_ROOT_PATH=/
export GCC_VER=`gcc -v 2>&1 | awk '/gcc version/ {print $3}'`
