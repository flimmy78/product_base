#
# CPSS common settings
#
##############################################################################
# CPSS related environment variables
##############################################################################
# USER_BASE - the directory where the CPSS code exists
# USER_MAKE - name of the make file to run for the different compilation flags
# COMPILATION_ROOT - directory to output the compiled fise and libs
#
# XBAR_VENDOR - the XBAR vendor                      -- empty / DUNE / PRESTERA
# FA_VENDOR - the FA vendor                          -- empty / PRESTERA
# BSP_VENDOR - the vendor of the BSP                 -- empty / DUNE
#
# NO_PP_USED - is it non-PP compilation ?            -- empty / NOT_USED
# EX_FAMILY  - is it Ex compilation                  -- empty / EXISTS
# MX_FAMILY  - is it Mx compilation                  -- empty / EXISTS
# CHX_FAMILY - is it Cheetah-X compilation           -- empty / EXISTS
# CH3_FAMILY - is it Cheetah-3 extra compilation     -- empty / EXISTS
# CH3P_FAMILY - is it Cheetah-3P extra compilation   -- empty / EXISTS
# TG_FAMILY  - is it Tiger compilation               -- empty / EXISTS
# TD_FAMILY  - is it Twist-D compilation             -- empty / EXISTS
# SAL_FAMILY - is it Salsa compilation               -- empty / EXISTS
# PM_FAMILY  - is it Puma compilation                -- empty / EXISTS
# EXDXMX_DRIVER - do we need the ex/mx/dx cpssDriver -- empty / EXISTS
#
# GT_SMI - do we need SMI support                    -- empty / EXISTS
# GT_PCI - do we need PCI support                    -- empty / EXISTS
# GT_I2C - do we need I2C support                    -- empty / EXISTS
#
# GALTIS_NO_APP_DEMO - do we compile and build image for 'Galtis without AppDemo'
#
# **************************************************************************************
#

export USER_BASE=$CPSS_PATH
export USER_MAKE=presteraTopMake

###########################################################
#  run set symbol for existing extensions
###########################################################
if [ "$HOST_WIN32" = "yes" ]; then
    EXT_PATH=`win32_path_to_posix "$CPSS_PATH"`/extension
else
    EXT_PATH=$CPSS_PATH/extension
fi

if [ "$mode" != "developer" ];then
 for fname in "$EXT_PATH"/*/setSymbol.sh; do
   if [ -f "$fname" -a -x "$fname" ];then     
        . "$fname"
   else
        echo " >> Wrong file name - $fname"      
   fi     
 done
fi

#############################################################

case $FAMILY in
    EX )
        export EX_FAMILY="EXISTS"
        export TG_FAMILY="EXISTS"
        export XBAR_VENDOR="PRESTERA"
        export FA_VENDOR="PRESTERA"
        ;;
    EX_ALL )
        export EX_FAMILY="EXISTS"
        export TG_FAMILY="EXISTS"
        export TD_FAMILY="EXISTS"
        ;;
    EX_TG)
        export EX_FAMILY="EXISTS"
        export TD_FAMILY="EXISTS"
        ;;
    EX_TD)
        export EX_FAMILY="EXISTS"
        export TD_FAMILY="EXISTS"
        ;;
    DX )
        export CHX_FAMILY="EXISTS"
        export CH3_FAMILY="EXISTS"
        ;;
    DXALL | DX_ALL)
        export CHX_FAMILY="EXISTS"
        export CH3_FAMILY="EXISTS"
        #export CH3P_FAMILY="EXISTS"
        #export SAL_FAMILY="EXISTS"
        ;;
    SAL | DX_SAL )
        export SAL_FAMILY="EXISTS"
        ;;
    DX_CHX )
        export CHX_FAMILY="EXISTS"
        ;;
    DX_CH3 )
        export CHX_FAMILY="EXISTS"
        export CH3_FAMILY="EXISTS"
        ;;
    DX_CH3P )
        export CHX_FAMILY="EXISTS"
        export CH3_FAMILY="EXISTS"
        export CH3P_FAMILY="EXISTS"
        ;;
    EX_DX_ALL )
        # All Ex devices
        export EX_FAMILY="EXISTS"
        export TG_FAMILY="EXISTS"
        export TD_FAMILY="EXISTS"
        # All Dx devices
        export CHX_FAMILY="EXISTS"
        export CH3_FAMILY="EXISTS"
        #export CH3P_FAMILY="EXISTS"
        ;;
    EX_TG_DX_SAL )
        # Tiger
        export EX_FAMILY="EXISTS"
        export TG_FAMILY="EXISTS"
        # Salsa
        export SAL_FAMILY="EXISTS"
        ;;
    EX_TG_DX_CH )
        # Tiger
        export EX_FAMILY="EXISTS"
        export TG_FAMILY="EXISTS"
        # Cheetah
        export CHX_FAMILY="EXISTS"
        ;;
    EXMXPM )
        export PM_FAMILY="EXISTS"
        ;;
    EXMXPM_DX_CH )
        export PM_FAMILY="EXISTS"
        export CHX_FAMILY="EXISTS"
        ;;
    NONE )
        export NO_PP_USED=NOT_USED
        ;;
    * )
        error_message "Unknown PP FAMILY $PP_FAMILY"
        return 1
        ;;
esac

if [[ ${EX_FAMILY} = "EXISTS" || ${PM_FAMILY} = "EXISTS" || ${CHX_FAMILY} = "EXISTS" \
   || ${SAL_FAMILY} = "EXISTS" || ${MX_FAMILY} = "EXISTS" ]]
then
    export EXDXMX_DRIVER="EXISTS"
fi

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
    PEX )
        #export GT_PEX="EXISTS"
        export GT_PCI="EXISTS"
        ;;
    * )
        error_message "Unknown MANAGEMENT BUS $MANAGEMENT_BUS"
        return 1
        ;;
esac

export BIN_BS=128
export BIN_SKIP=1
export AUTO_INIT_SYS=ENABLE
unset  DPSS_CONFIG

case $XBAR_TYPE in
    DUNE )
        export XBAR_VENDOR=DUNE
        export FA_VENDOR=DUNE
        export BSP_VENDOR=DUNE
        ;;
    FOX )
        export XBAR_VENDOR=PRESTERA
        export FA_VENDOR=PRESTERA
        ;;
    CAP )
        export XBAR_VENDOR=PRESTERA
        ;;
    NONE )
        ;;
    * )
        error_message "Unsupported argument: $OPTARG for -x option."
        exit 1
        ;;
esac


if [ "$BSP_VENDOR" = "DUNE" ]
then
# this is image for 98FX9130 back plain board without PPp
# both extentions and UT are not needed    
    export INC_EXTENSION_SYMBOL=NO
    export INC_EXTENSION_SYMBOL1=NO
    export INC_EXTENSION_SYMBOL_G3=NO
    export INCLUDE_UTF=NO
fi

. $tool_build/product/simulation.inc
