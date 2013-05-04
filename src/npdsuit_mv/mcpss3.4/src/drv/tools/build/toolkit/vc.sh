# toolkit defs

# call "c:\Program Files\Microsoft Visual Studio\VC98\Bin\VCVARS32.BAT"
win32_call_bat_file VCVARS32 "c:\Program Files\Microsoft Visual Studio\VC98\Bin\VCVARS32.BAT"
export TOOL=visual
export VC_VER=6
export CPU=VC

export CPU_TOOL=intel
export BIN_BS=128
export BIN_SKIP=1
export WM_USED=USED

unset WIND_BASE
unset CC_ARCH
unset PROJ_BSP_DIR
