#

. $tool_build/product/cpss_common.inc
case $FAMILY in
    EXMXPM)
        BUILD_ONLY="mainPpDrv src/cpss cpss/exMxPm exMxPm/exMxPmGen exMxPmGen/ipLpmEngine"
        ;;
    EX)
        BUILD_ONLY="mainPpDrv src/cpss cpss/exMx exMx/exMxGen exMxGen/ipLpmEngine"
        ;;
    *)
        echo "No IP libraries for family $FAMILY"
        exit 0
esac

LINUX_BUILD_KERNEL=NO
DONT_LINK=YES

export BUILD_ONLY
