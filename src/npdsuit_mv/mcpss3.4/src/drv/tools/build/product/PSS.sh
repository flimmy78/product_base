#

. $tool_build/product/pss_common.inc

export PRODUCT_TYPE="PSS"        
export DX_BUILD_TYPE="OLD"
export SECOND_FAMILY="NOT_EXISTS"

case "$FAMILY" in
    EX | EXPSS)
        export PSS_CORE_EX="EXISTS"
        export PSS_CORE_FA="EXISTS"
        export PSS_CORE_XBAR="EXISTS"
        export NO_CPSS_WRAPPERS="EXISTS"
        ;;
    DX | MX | EXMXPM | DXSX )
    ;;
    *)
        error_message "wrong family: ${FAMILY}"
        exit 1
    ;;
esac

export PP_FAMILY=$FAMILY
export PRESTERA_FAMILY=${FAMILY}_FAMILY
