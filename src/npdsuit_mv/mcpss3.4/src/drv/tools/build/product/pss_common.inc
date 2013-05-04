#

export USER_BASE=$PSS_PATH
export USER_HOME="$PSS_PATH"
export USER_MAKE=presteraTopMake

# if MANAGEMENT_BUS_PARAM is not set by -b options
# By default we set MANAGEMENT_BUS_PARAM to PCI, except of Dx on ARM5181
if [ -z "$MANAGEMENT_BUS_PARAM" ]; then
    if [ "$CPU_BOARD" = "ARM5181" ] && [ "$CHX_FAMILY" = "EXISTS" ]
    then 
        MANAGEMENT_BUS_PARAM=PCI_SMI
     else
        MANAGEMENT_BUS_PARAM=$DEFAULT_MANAGEMENT_BUS
    fi
fi

case $MANAGEMENT_BUS_PARAM in
    PCI )
        export GT_PCI="EXISTS"
        export MANAGEMENT_BUS=GT_MNG_PCI
        ;;
    SMI )
        export GT_SMI="EXISTS"
        export MANAGEMENT_BUS=GT_MNG_SMI
        ;;
    PCI_SMI )
        export GT_PCI="EXISTS"
        export GT_SMI="EXISTS"
        export MANAGEMENT_BUS=GT_MNG_PCI_SMI
        ;;
    PCI_SMI_I2C )
        export GT_PCI="EXISTS"
        export GT_SMI="EXISTS"
        export GT_I2C="EXISTS"
        export MANAGEMENT_BUS=GT_MNG_PCI_SMI_I2C
        ;;
    * )
        error_message "Unknown MANAGEMENT BUS $MANAGEMENT_BUS_PARAM"
        exit 1
        ;;
esac

export BIN_BS=128
export BIN_SKIP=1
export AUTO_INIT_SYS=ENABLE

. $tool_build/product/simulation.inc
