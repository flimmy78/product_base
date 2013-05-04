#
# required environment:
# 
#

os_pre_link()
{
    # prepare to link
    if [ -f simulationLibsCopy.bat ]
    then
        simulationLibsCopy.bat
    fi
    return 0
}

os_link()
{
    # link project
    info_message "gmake -C ${USER_BASE} -sf MakefileWin.mak application"
    #info_message "PROJ_BSP_DIR - $PROJ_BSP_DIR"
    gmake -C ${USER_BASE} -sf MakefileWin.mak application

    if [ -f "`win32_path_to_posix $PROJ_BSP_DIR`"/appDemoSim.exe ]
    then
          echo "***************************"
          echo "****    SUCCESS !!!    ****"
          echo "***************************"
    fi
    return 0
}

os_post_build()
{
    # copy vxWorks.st
    if [ -z "$IMAGE_PATH" -o "$IMAGE_PATH" = "-" ]; then
        return 0
    fi

    mkdir -p "$IMAGE_PATH" >/dev/null 2>&1
    info_message "copy $PROJ_BSP_DIR\\appDemoSim.exe $IMAGE_PATH"
    cmd /c "copy $PROJ_BSP_DIR\\appDemoSim.exe $IMAGE_PATH" >/dev/null 2>&1
    cmd /c "copy $PROJ_BSP_DIR\\appDemoSim.map $IMAGE_PATH" >/dev/null 2>&1
    cmd /c "copy $PROJ_BSP_DIR\\appDemoSim.pdb $IMAGE_PATH" >/dev/null 2>&1
    return 0
}

# os definitions
export OS_RUN=win32
unset RTOS_ON_SIM
unset DISTRIBUTED_SIMULATION_ROLE
export ASIC_SIMULATION=EXISTS
export DESTINATION_BSP_DIR="$COMPILATION_ROOT\\application\\work\\$TOOLKIT"
export PROJ_BSP_DIR="$DESTINATION_BSP_DIR"
