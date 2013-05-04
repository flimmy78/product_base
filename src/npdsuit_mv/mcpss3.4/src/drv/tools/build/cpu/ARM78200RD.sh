# ARM78200RD.sh
# Target CPU settings

. $tool_build/cpu/ARM78200.sh

export CPU_DIR=ARM78200RD
export ARM_CPU=78200RD
#
DEFAULT_TOOLKIT_Linux=gnueabi
CPU_PRE_TOOLKIT_gnueabi=cpu_pre_toolkit_gnueabi
#
if [ $TARGET_OS = Linux ]
then
    DEFAULT_LINUX_LSP_NAME=mv78200_amc_lsp_2_6
    DEFAULT_LINUX_CONFIG=mv78200rd_defconfig
fi

#
# per toolkit settings
# executed _after_ toolkit choosen,
# overrides defaults
cpu_pre_toolkit_tornado()
{
    ##
    ## WIND_BASE can be set by user
    ##
    if [ -z $WIND_BASE ]
    then
       export WIND_BASE='c:\TornadoARM'
       echo "WIND_BASE - $WIND_BASE"
    else   
       ### check if exists WIND_BASE defined by user 
       if [ -d $WIND_BASE ]
       then
          echo "WIND_BASE - $WIND_BASE"
       else
          echo "Wrong WIND_BASE - $WIND_BASE"
          exit 0
       fi
    fi

    export PROJ_BSP_DIR="$BSP_CONFIG_DIR"'\mv78200rd_amc_bsp'
    export CC_ARCH=-march=armv5
    export CC_ARCH_DIAB=-tARMV5LN:vxworks55
	export BOARD=LION_RD
}
linux_2_6_cr_jffs2_params()
{
    UIMAGE_PATH="${DIST}/arch/${CPU_TOOL}/boot"
    UIMAGE_NO_COPY=TRUE
    CR_JFFS2_FILE_NAME="cr_jffs2_img_discoduo_lionrd.sh"
    CR_JFFS2_PARAMS="${DIST}/arch/${CPU_TOOL}/boot/uImage ${DIST}/rootfs little_endian ${IMAGE_PATH}"
}
