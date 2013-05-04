# toolkit defs

export TOOL_FAMILY=gnu
case $CPU_BOARD in
    PPC603 | PPC603_VB | PPC603_SYCAMOR )
        COMP_PREFIX=ppc_82xx-
        CC_ARCH=-mcpu=603
        XCOMP_ROOT_PATH_SUFFIX=ppc_82xx
        ;;
    PPC85XX )
        COMP_PREFIX=ppc_85xx-
        XCOMP_ROOT_PATH_SUFFIX=ppc_85xx
        ;;
    PPC604 )
        CC_ARCH=-mcpu=604
        COMP_PREFIX=ppc_7xx-
        XCOMP_ROOT_PATH_SUFFIX=ppc_7xx
        ;;
    ARM_EV | ARM_RD | ARM_SP | ARM_GM )
        COMP_PREFIX=arm-elf
        XCOMP_ROOT_PATH_SUFFIX=arm
        ;;
    ARM5181 | ARM5281)
        COMP_PREFIX=arm-linux-
        XCOMP_ROOT_PATH_SUFFIX=arm
        ;;
    *)
        error_message "CPU $CPU_BOARD is unsuppoted by $TOOLKIT toolkit"
        exit 1
        ;;
esac

export COMP_PREFIX
COMPILER=${COMP_PREFIX}gcc

export XCOMP_ROOT_PATH_SUFFIX

TOOLKIT_PATH=`which $COMPILER`
TOOLKIT_PATH=`dirname $TOOLKIT_PATH`
TOOLKIT_PATH=`dirname $TOOLKIT_PATH`
export COMPILER_ROOT=$TOOLKIT_PATH
TOOLKIT_PATH=`dirname $TOOLKIT_PATH`
export TOOL_CHAIN_PATH=$TOOLKIT_PATH
export XCOMP_INC_PATH=$TOOLKIT_PATH
export XCOMP_ROOT_PATH=$TOOLKIT_PATH/$XCOMP_ROOT_PATH_SUFFIX
export GCC_VER=`$COMPILER -v 2>&1 | awk '/gcc version/ {print $3}'`
