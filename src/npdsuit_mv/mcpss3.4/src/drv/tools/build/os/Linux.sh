#
# required environment:
#   LINUX_DIST_PATH
#   LSP_USER_BASE
#   LSP_SUBDIR            try $LSP_USER_BASE/$LSP_SUBDIR/$LINUX_LSP_NAME first
#                         Otherwise use $LSP_USER_BASE/$LINUX_LSP_NAME
#   LINUX_KERNEL_VERSION  (or DEFAULT_LINUX_KERNEL_VERSION from CPU defs)
#   LINUX_LSP_NAME        (or DEFAULT_LINUX_LSP_NAME from CPU defs)
#   LINUX_BUILD_KERNEL    set to YES to build kernel. Default is YES
# 
#

LINUX_BUILD_KERNEL=${LINUX_BUILD_KERNEL:-YES}


os_linux_patch_kernel()
{
    # extract and patch kernel if required
    cd $LINUX_DIST_PATH

    PACKAGE=linux-$LINUX_KERNEL_VERSION
    if [ -f $PACKAGE".tar.bz2" ]
    then
         EXT=.tar.bz2
         COMPRESSION=j
    elif [ -f $PACKAGE".tar.gz" ]
    then
        EXT=.tar.gz
        COMPRESSION=z
    else
       error_message -e "\tLinux kernel zip not found"
       error_message -e "Please put linux kernel zip file version $LINUX_KERNEL_VERSION to directory $LINUX_DIST_PATH"
       return 1
    fi

    # APPLY LSP_SUBDIR
    if [ "$LSP_SUBDIR" != "" -a -d $LSP_USER_BASE/$LSP_SUBDIR/$LINUX_LSP_NAME ]
    then
        LSP_USER_BASE=$LSP_USER_BASE/$LSP_SUBDIR
    fi

    if [ \! -d $CPU_BOARD/$LINUX_LSP_NAME ]
    then
        mkdir -p $CPU_BOARD/$LINUX_LSP_NAME ||
        {
            error_message "can't mkdir $CPU_BOARD/$LINUX_LSP_NAME"
            return 1
        }
    fi
    if [ -f $DIST/.linux_config_done ]
    then
        # check LSP match
        lsp="`find ${LSP_USER_BASE}/${LINUX_LSP_NAME}/. -type f -print0 | sort -z |
            xargs -0 cat | md5sum | cut -d ' ' -f 1`"
        if [ "$lsp" = "`cat $DIST/.linux_config_done`" ]
        then
            rm -f $DIST/rootfs/lib/modules/mv*.ko
            rm -f $DIST/rootfs/usr/bin/appDemo
            rm -f $DIST/rootfs/usr/bin/appImage
            info_message -e "END configLinux  -> was done before"
            return 0
        fi
    fi

    # kernel not yet compiled or lsp doesn't match
    trace_message -e "\trm -rf "$DIST
    rm -rf $DIST/.[^.]* $DIST/*
    
    cd $CPU_BOARD/$LINUX_LSP_NAME
    trace_message -e "tar x${COMPRESSION}f $LINUX_DIST_PATH/${PACKAGE}${EXT}"
    tar x${COMPRESSION}f $LINUX_DIST_PATH/${PACKAGE}${EXT}
    if [ $? -ne 0 ]
    then
      error_message -e "\tTAR failed"
      return 1
    else
        info_message -e "\ttar ...done"
    fi

    cd $LINUX_DIST_PATH

    #Dist O.K ?
    if [ ! -d $DIST ]
    then
      error_message -e "\tDISTRIBUTION NAME ?"
      return 1
    fi

    #Apply the LSP
    #trace_message  "cp -rf --reply=yes ${LSP_USER_BASE}/${LINUX_LSP_NAME}/* ${DIST}"
    #cp -rf --reply=yes ${LSP_USER_BASE}/${LINUX_LSP_NAME}/* ${DIST}
    trace_message  "yes | cp -rfL ${LSP_USER_BASE}/${LINUX_LSP_NAME}/* ${DIST}"
    yes | cp -rfL ${LSP_USER_BASE}/${LINUX_LSP_NAME}/* ${DIST}
    if [ $? -ne 0 ]
    then
    error_message -e "\tCopy LSP failed"
    return 1
    else
        info_message -e "\tCopy LSP...done"
    fi

    # patch kernel
#   if [ -f $tool_build/os/Linux_patch_kernel.sh ]
#    then
#        sh $tool_build/os/Linux_patch_kernel.sh
#        if [ -f $DIST/kernel_patched_env.sh ]
#        then
#            . $DIST/kernel_patched_env.sh
#        fi
#    fi

    # make oldconfig, copy our configuration
    trace_message  "cp ${DIST}/arch/${CPU_TOOL}/configs/${DEFCONFIG} ${DIST}/.config"
    cp ${DIST}/arch/${CPU_TOOL}/configs/${DEFCONFIG} ${DIST}/.config
    if [ $? -ne 0 ]
    then
        error_message -e "\tCopy old config failed"
        return 1
    else
        info_message -e "\tCopy old config...done"
    fi

    return 0
}

os_linux_kernel_is_already_built()
{
    if [ -f $DIST/.linux_config_done ]
    then
      info_message -e "\t$DIST/.linux_config_done exists; kernel is already built"
      return 0
    fi
    info_message -e "\t$DIST/.linux_config_done NOT exists; kernel is NOT built"  
    return 1
}

os_linux_make_rootfs()
{
    # Validate that Busybox tarball exists
    if [ ! -f busybox-1.01.tar.bz2 -a ! -f busybox-1.01.tar.gz ]
    then
        error_message -e "\tBusybox tarball missing"
        return 1
    else
        trace_message -e "\tBusybox tarball found"
    fi

    trace_message -e "\tmkdir "${DIST}"/rootfs"
    mkdir ${DIST}/rootfs
    if [ $? -ne 0 ]
    then
        error_message -e "\tfailed make rootfs directory"
        return 1
    else
        info_message -e "\tmake rootfs...done"
    fi
    return 0
}

os_link()
{
    # compile project
    info_message "${MAKE} ${MAKE_FLAGS} -C ${USER_BASE} -f Makefile ${MAKE_TARGET_LINK:-application}"
    ${MAKE} ${MAKE_FLAGS} -C ${USER_BASE} -f Makefile ${MAKE_TARGET_LINK:-application}
    if [ $? -ne 0 ] # test result
    then
        error_message -e "\tfailed to build $USER_BASE"
        return 1
    fi
    return 0
}

# os definitions
export OS_RUN=linux
export OS_TARGET=Linux
export OS_TARGET_RELEASE=2
LINUX_KERNEL_VERSION=${LINUX_KERNEL_VERSION:-$DEFAULT_LINUX_KERNEL_VERSION}
LINUX_LSP_NAME=${LINUX_LSP_NAME:-$DEFAULT_LINUX_LSP_NAME}
export DIST=$LINUX_DIST_PATH/$CPU_BOARD/$LINUX_LSP_NAME/linux-${LINUX_KERNEL_VERSION}
export DIST_HOME=${DIST}
export LINUX_BASE=${DIST}

case $LINUX_KERNEL_VERSION in
    2.6.12.5 | 2.6.14 | 2.6.22.18 | 2.6.31.8)
        KPATCHLEVEL=6
        ;;
    2.4.29)
        KPATCHLEVEL=4
        # no patch required to export syscall table
        export KERNEL_SYSCALL_TABLE_EXPORTED=yes
        ;;
    *)
        error_message "Wrong KRELEASE -  $KRELEASE"
        exit 1
        ;;
esac
export KPATCHLEVEL

if [ -z "$DEFAULT_LINUX_CONFIG" ];
then
    error_message "DEFAULT_LINUX_CONFIG not defined in $tool_build/cpu/$CPU_BOARD.sh"
    exit 1
fi
DEFCONFIG=${DEFAULT_LINUX_CONFIG}

. $tool_build/os/Linux_2.${KPATCHLEVEL}.inc
