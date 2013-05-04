#

. $tool_build/product/pss_common.inc


export PRODUCT_TYPE="CPSS"
export PP_FAMILY=$FAMILY
export INCLUDE_UTF="YES"

unset EXCLUDE_XBAR
unset PSS_CORE_XBAR
unset PSS_CORE_EX
unset PSS_CORE_FA

if [ -d "$CPSS_PATH" ]
then
    export CPSS_USER_BASE=$CPSS_PATH
fi

case $PP_FAMILY in
EX)
    export EXDXMX_DRIVER="EXISTS"
    export TG_FAMILY="EXISTS"
    export CPSS_EX_TG="EXISTS"
    export CPSS_EX="EXISTS"
    export PRESTERA_FAMILY="EX_FAMILY"
    export XBAR_VENDOR="PRESTERA"
    export FA_VENDOR="PRESTERA"
# access to cpss galtis wrappers  
    export EX_FAMILY="EXISTS"
    ;;

DX | DXSX | DXDPSS)
    export DX_BUILD_TYPE="POC"
    export EXDXMX_DRIVER="EXISTS"
    export CHX_FAMILY="EXISTS"
    export CH3_FAMILY="EXISTS"
    export CPSS_CH="EXISTS"
    if [ "$GT_SMI" = "EXISTS" -a $PP_FAMILY = DXSX ]; then
        export SAL_FAMILY="EXISTS"
        export CPSS_SAL="EXISTS"
    fi
    export PRESTERA_FAMILY="DX_FAMILY"
    export EXCLUDE_XBAR="ON"
    if [ "$PP_FAMILY" = "DXDPSS" ];then
       export PSS_MODE="PSS_DPSS"
       export DPSS_DIR="T2CDPSS"
    fi   
    ;;

EXMXPM)
    export PM_FAMILY="EXISTS"
    export EXDXMX_DRIVER="EXISTS"
    export CPSS_EXMXPM="EXISTS"
    export PRESTERA_FAMILY="EXMXPM_FAMILY"
    export EXT_XBAR_VENDOR=CPSS
    export PSS_CORE_FA=EXISTS
    export PSS_CORE_XBAR=EXISTS
    export PM_FAMILY=EXISTS
    export PRESTERA_FAMILY=EXMXPM_FAMILY

    ;;

*)
    error_message "Wrong PP_FAMILY - ${PP_FAMILY}"
    exit 1
    ;;
esac
