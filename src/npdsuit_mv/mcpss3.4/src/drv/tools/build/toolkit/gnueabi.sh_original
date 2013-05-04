# toolkit defs

export TOOL_FAMILY=gnu
export USE_GNUEABI=TRUE

export COMP_PREFIX=arm-none-linux-gnueabi-
COMPILER=${COMP_PREFIX}gcc

export XCOMP_ROOT_PATH_SUFFIX=arm-none-linux-gnueabi/sys-root      

TOOLKIT_PATH=`which $COMPILER`
TOOLKIT_PATH=`dirname $TOOLKIT_PATH`
export COMPILER_ROOT=$TOOLKIT_PATH
export TOOL_CHAIN_PATH=$TOOLKIT_PATH
TOOLKIT_PATH=`dirname $TOOLKIT_PATH`
export XCOMP_INC_PATH=$TOOLKIT_PATH
export XCOMP_ROOT_PATH=$TOOLKIT_PATH/$XCOMP_ROOT_PATH_SUFFIX
export GCC_VER=`$COMPILER -v 2>&1 | awk '/gcc version/ {print $3}'`
