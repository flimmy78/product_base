# toolkit defs

# call "C:\Program Files\Microsoft Visual Studio 8\VC\bin\vcvars32.bat"
win32_call_bat_file VCVARS32 "C:\\Program Files\\Microsoft Visual Studio 8\\VC\\bin\\vcvars32.bat"
export TOOL=visual
export VC_VER=8
export CPU=VC8

export CPU_TOOL=intel
export BIN_BS=128
export BIN_SKIP=1
export WM_USED=USED

unset WIND_BASE
unset CC_ARCH
unset PROJ_BSP_DIR
