# toolkit defs

export TOOL_FAMILY=gnu

case $CPU_BOARD in
    PPC85XX )
        COMP_PREFIX=ppc-fbsd-e500-
        XCOMP_ROOT_PATH_SUFFIX=ppc-fbsd-e500
        ;;
    *)
        error_message "CPU $CPU_BOARD is unsuppoted by $TOOLKIT toolkit"
        exit 1
        ;;
esac

export COMP_PREFIX
export XCOMP_ROOT_PATH_SUFFIX
export COMPILER=${COMP_PREFIX}gcc
export CC=${COMP_PREFIX}gcc
export LD="NO_MV_FRONTEND=yes ${COMP_PREFIX}ld"
export NM=${COMP_PREFIX}nm
export OBJCOPY=${COMP_PREFIX}objcopy





TOOLKIT_PATH=`which $COMPILER`
TOOLKIT_PATH=`dirname $TOOLKIT_PATH`
TOOLKIT_PATH=`dirname $TOOLKIT_PATH`
export COMPILER_ROOT=$TOOLKIT_PATH
TOOLKIT_PATH=`dirname $TOOLKIT_PATH`
export TOOL_CHAIN_PATH=$TOOLKIT_PATH
export XCOMP_INC_PATH=$TOOLKIT_PATH
export XCOMP_ROOT_PATH=$TOOLKIT_PATH/$XCOMP_ROOT_PATH_SUFFIX
export GCC_VER=`$COMPILER -v 2>&1 | awk '/gcc version/ {print $3}'`
