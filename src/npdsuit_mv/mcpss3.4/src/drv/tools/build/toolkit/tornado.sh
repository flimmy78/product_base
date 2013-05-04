# toolkit defs

export TOOL=${TOOL:-gnu}

case $CPU_BOARD in
    ARM5181)
        ;;
    ARM5281)
        ;;
esac

export WIND_HOST_TYPE=x86-win32
if [ "$TOOL" = "gnu" ]; then
    win32_add_paths "$WIND_BASE\\host\\$WIND_HOST_TYPE\\bin"
    export COMPILER=GNU
elif [ "$TOOL" = "diab" ]; then
    # Diab toolchain addition
    export DIABLIB="$WIND_BASE\\host\\diab"
    win32_add_paths "$DIABLIB\\WIN32\\bin"
    export COMPILER=WBC
else
    error_message "Wrong TOOL:$tool"
fi
