# toolkit defs

export TOOL_FAMILY=gnu
export USE_MV5SFT_BE=TRUE

export COMP_PREFIX=armeb-mv5sft-linux-gnueabi-
COMPILER=${COMP_PREFIX}gcc

export XCOMP_ROOT_PATH_SUFFIX=armeb-mv5sft-linux-gnueabi/sys-root      

TOOLKIT_PATH=`which $COMPILER`
TOOLKIT_PATH=`dirname $TOOLKIT_PATH`
export COMPILER_ROOT=$TOOLKIT_PATH
export TOOL_CHAIN_PATH=$TOOLKIT_PATH
TOOLKIT_PATH=`dirname $TOOLKIT_PATH`
export XCOMP_INC_PATH=$TOOLKIT_PATH
export XCOMP_ROOT_PATH=$TOOLKIT_PATH/$XCOMP_ROOT_PATH_SUFFIX
export GCC_VER=`$COMPILER -v 2>&1 | awk '/gcc version/ {print $3}'`
