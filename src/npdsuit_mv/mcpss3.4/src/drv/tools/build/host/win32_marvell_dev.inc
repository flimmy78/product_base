# host definitions Marvell internal environment
# (WIN32)
#
# Environment variables:
#
#   DRIVE   - drive where pss/cpss projects located. Default X:
#
#       Version number - $ 7

. $tool_build/host/win32.sh

# set the driver to use
export DRIVE="${DRIVE:-X:}"

# Read the config spec info
export TOOLS_BASE='F:\Objects\pss'
cmd /c 'F:\UTILS\B_UTILS\V2\ReadCS.exe'
# read and parse config file
# sample:
#   @set CC_VIEW_TYPE=DeveloperView
#   @set CC_CURRENT_CHANGE_SET=pss_1.9_build_aharong_3
#   @set CC_CURRENT_COMPONENT=pss
#   @set CC_CURRENT_BASE_LINE=PSS_1.9_033
#   @set CC_RELEASE_STREAM=pss_1.9
config="`win32_path_to_posix 'C:\temp\catcs_pss.bat'`"
while read line
do
   cmd="${line%% *}"
   if [ "$cmd" = "@set" -o "$cmd" = "set" ]; then
       eval ${line#*set }
   fi
done < "$config"
cmd /c 'del C:\temp\catcs_pss.bat'


check_create_dir() {
    win32_path_exists "$1" || cmd /c "mkdir $1"
}

copy_baseline_objects() {

    check_create_dir "$COMPILATION_ROOT\\$1\\libs\\$2"
    xcopy "$OBJ_NETWORK_ROOT\\$1\\libs\\$2\\*.*" "$COMPILATION_ROOT\\$1\\libs\\$2\\" /E /H /R /Y /Q

    check_create_dir "$COMPILATION_ROOT\\$1\\objTemp\\$2"
    xcopy "$OBJ_NETWORK_ROOT\\$1\\objTemp\\$2\\*.*" "$COMPILATION_ROOT\\$1\\objTemp\\$2\\"  /E /H /R /Y /Q
}

copy_baseline_extension_objects() {

    check_create_dir "$COMPILATION_ROOT\\extension\\$1\\$2\\libs\\$3"
    xcopy "$OBJ_EXTENSIONS_NETWORK_ROOT\\extensions\\$1\\$family_subdir\\extension\\$1\\$2\\libs\\$3\\*.*" "$COMPILATION_ROOT\\extension\\$1\\$2\\libs\\$3\\" /E /H /R /Y /Q

    check_create_dir "$COMPILATION_ROOT\\extension\\$1\\$2\\objTemp\\$3"
    xcopy "$OBJ_EXTENSIONS_NETWORK_ROOT\\extensions\\$1\\$family_subdir\\extension\\$1\\$2\\objTemp\\$3\\*.*" "$COMPILATION_ROOT\\extension\\$1\\$2\\objTemp\\$3\\"  /E /H /R /Y /Q
}


win32_marvell_BSP_detect_paths()
{
    makefile_location_type=generic
               
    # BSP/GM locations
    case ${CPU_BOARD}_${TOOLKIT} in
        i386_vc)
            CC_BSP_DIR="$DRIVE\\pss\\SW\\application\\work\\vc"
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\application\\work\\vc"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\application\\work\\vc"
            GM_DLL_VC_DIR="$USER_BASE\\simulation\\libs\\GM\\vc6"
            GM_DLL_VC_ROOT="$GM_DLL_ROOT\\vc6"
            TMP_OBJ_LIB_DIR_NAME="VC"
            ;;
        i386_vc8)
            CC_BSP_DIR="$DRIVE\\pss\\SW\\application\\work\\vc8"
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\application\\work\\vc8"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\application\\work\\vc8"
            GM_DLL_VC_DIR="$USER_BASE\\simulation\\libs\\GM\\vc8"
            GM_DLL_VC_ROOT="$GM_DLL_ROOT\\vc8"
            TMP_OBJ_LIB_DIR_NAME=$i386
            ;;
        i386_bc)
            OPTIONS="$OPTIONS -T bc"
            CC_BSP_DIR="$DRIVE\\pss\\SW\\application\\work\\bc"
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\application\\work\\bc"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\application\\work\\bc"
            GM_DLL_BC_DIR=""
            GM_DLL_BC_ROOT=""
            TMP_OBJ_LIB_DIR_NAME="BC"
            ;;

        PPC604_tornado)
            TMP_OBJ_LIB_DIR_NAME="PPC604"
            CC_BSP_DIR="$DRIVE\\tornado\\target\\EV_64260A\\config\\EV-64260A-750cx-PCI"
            UNPACKED_BSP_DIR="$CPSS_PATH\\config\\EV-64260A-750cx-PCI"    
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\EV-64260A-750cx-PCI"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\EV-64260A-750cx-PCI"
            ;;
        PPC603_tornado)
            TMP_OBJ_LIB_DIR_NAME="PPC603"
            CC_BSP_DIR="$DRIVE\\tornado\\target\\mv_pmc8245_bsp\\config\\mv_pmc8245"
            UNPACKED_BSP_DIR="$CPSS_PATH\\config\\mv_pmc8245"    
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\mv_pmc8245"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\mv_pmc8245"
            ;;
        PPC603_wb_diab | PPC603_tornado_diab)
            TMP_OBJ_LIB_DIR_NAME="PPC603"
            CC_BSP_DIR="$DRIVE\\tornado\\target\\mv_pmc8245_bsp\\config\\mv_pmc8245_Tornado2.2.1"
            UNPACKED_BSP_DIR="$CPSS_PATH\\config\\mv_pmc8245_Tornado2.2.1"    
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\mv_pmc8245_Tornado2.2.1"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\mv_pmc8245_Tornado2.2.1"
            ;;
        PPC85XX_workbench26 | PPC85XX_wb_diab | PPC85XX_workbench26_diab )
            TMP_OBJ_LIB_DIR_NAME="PPC85XX"
            CC_BSP_DIR="$DRIVE\\tornado\\target\\gda8548_bsp\\config\\GDA8548_6.5"
            UNPACKED_BSP_DIR="$CPSS_PATH\\config\\GDA8548_6.5"    
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\GDA8548_6.5"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\GDA8548_6.5"
            if [ "$TOOLKIT" = "wb_diab" -o "$TOOLKIT" = "workbench26_diab" ]; then
                TMP_OBJ_LIB_DIR_NAME="PPC85XX_DIAB"
                DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\GDA8548_6.5_DIAB"
                BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\GDA8548_6.5_DIAB"
            fi
            ;;
        PPC85XX_LION_RD_workbench26 | PPC85XX_LION_RD_wb_diab | PPC85XX_LION_RD_workbench26_diab )
            TMP_OBJ_LIB_DIR_NAME="PPC85XX"
            CC_BSP_DIR="$DRIVE\\tornado\\target\\lion_mpc8544_bsp\\config"
            UNPACKED_BSP_DIR="$CPSS_PATH\\config\\lion_mpc8544_bsp"
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\lion_mpc8544_bsp"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\lion_mpc8544_bsp"
            if [ "$TOOLKIT" = "wb_diab" -o "$TOOLKIT" = "workbench26_diab" ]; then
                TMP_OBJ_LIB_DIR_NAME="mpc8544_DIAB"
                DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\lion_mpc8544_DIAB_bsp"
                BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\lion_mpc8544_DIAB_bsp"
            fi
            ;;
        ARM_EV_tornado)
            TMP_OBJ_LIB_DIR_NAME="ARM946ES"
            CC_BSP_DIR="$DRIVE\\tornado\\target\\88e6218_bsp\\config\\88e6218"
            UNPACKED_BSP_DIR="$CPSS_PATH\\config\\88e6218"    
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\88e6218"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\88e6218"
            makefile_location_type=ARM_EV
            ;;
        ARM_RD_tornado)
            TMP_OBJ_LIB_DIR_NAME="ARM946ES"
            CC_BSP_DIR="$DRIVE\\tornado\\target\\rd_dx24x_rdln_bsp\\config\\rd_dx24x"
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\rd_dx24x"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\rd_dx24x"
            makefile_location_type=ARM_RD
            ;;
        PPC603_VB_tornado)
            TMP_OBJ_LIB_DIR_NAME="PPC603"
            CC_BSP_DIR="$DRIVE\\tornado\\target\\rd_ex126_vb48g_bsp\\config\\rd_ex126_vb48g"
            UNPACKED_BSP_DIR="$CPSS_PATH\\config\\rd_ex126_vb48g"    
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\rd_ex126_vb48g"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\rd_ex126_vb48g"
            ;;
        ARM5181_tornado)
            TMP_OBJ_LIB_DIR_NAME="ARM5181"
            CC_BSP_DIR="$DRIVE\\tornado\\target\\db_88f5181_prpmc_bsp\\config\\db_88f5181_prpmc"
            UNPACKED_BSP_DIR="$CPSS_PATH\\config\\db_88f5181_prpmc"    
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\db_88f5181_prpmc"
            BSP_NETWORK_MAKE_DIR="$OBJ_NETWORK_ROOT\\config\\db_88f5181_prpmc\\swControllers\\VxBsp\\"            
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\db_88f5181_prpmc"
            makefile_location_type=ARM5181
            ;;
        ARM5281_tornado)
            TMP_OBJ_LIB_DIR_NAME="ARM5281"
            CC_BSP_DIR="$DRIVE\\tornado\\target\\db_88f5281_mng_bsp\\config\\db_88f5281_mng"
            UNPACKED_BSP_DIR="$CPSS_PATH\\config\\db_88f5281_mng"    
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\db_88f5281_mng"
            BSP_NETWORK_MAKE_DIR="$OBJ_NETWORK_ROOT\\config\\db_88f5281_mng\\swControllers\\VxBsp\\"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\db_88f5281_mng"
            makefile_location_type=ARM5281
            ;;
        ARM5281_workbench26 | ARM5281_wb_diab | ARM5281_workbench26_diab)
            TMP_OBJ_LIB_DIR_NAME="ARM5281W"
            CC_BSP_DIR="$DRIVE\\tornado\\target\\db_88f5281_mng_bsp\\config\\db_88f5281_mng_WB"
            UNPACKED_BSP_DIR="$CPSS_PATH\\config\\db_88f5281_mng_WB"    
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\db_88f5281_mng_WB"
            BSP_NETWORK_MAKE_DIR="$OBJ_NETWORK_ROOT\\config\\db_88f5281_mng_WB\\swControllers\\VxBsp\\"            
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\db_88f5281_mng_WB"
            makefile_location_type=ARM5281
            if [ "$TOOLKIT" = "wb_diab" -o "$TOOLKIT" = "workbench26_diab" ]; then
                TMP_OBJ_LIB_DIR_NAME="ARM5281W_DIAB"
                DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\db_88f5281_mng_WB_DIAB"
                BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\db_88f5281_mng_WB_DIAB"
            fi
            ;;
        ARM6281_DB_xxx)
            # TODO: unknown toolkit
            TMP_OBJ_LIB_DIR_NAME="ARM5281"
            CC_BSP_DIR="$DRIVE\\tornado\\target\\db_88f6281_bp_bsp\\config\\db_88f6281_bp"
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\db_88f6281_bp"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\db_88f6281_bp"
            ;;
        XCAT_tornado | XCAT_BE_tornado)
            TMP_OBJ_LIB_DIR_NAME="ARM926EJ"
         ## CC_BSP_DIR="$DRIVE\\tornado\\target\\xcat_bsp\\config\\xcat_bsp"
            CC_BSP_DIR="$DRIVE\\switching_infra\\bsp\\xcat_bsp"
            UNPACKED_BSP_DIR="$CPSS_PATH\\config\\xcat_bsp"    
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\xcat_bsp"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\xcat_bsp"
            if [ "$CPU_BOARD" = XCAT_BE ]; then
                TMP_OBJ_LIB_DIR_NAME="ARM926EJ_BE"
                BSP_NETWORK_DIR="${BSP_NETWORK_DIR}BE"
                DESTINATION_BSP_DIR="${DESTINATION_BSP_DIR}BE"
            fi
            ;;
        ARM78200_tornado | ARM78200_BE_tornado)
            TMP_OBJ_LIB_DIR_NAME="ARM78200"
            CC_BSP_DIR="$DRIVE\\tornado\\target\\mv78200_amc_bsp\\config\\mv78200_amc_bsp"
            UNPACKED_BSP_DIR="$CPSS_PATH\\config\\mv78200_amc_bsp"    
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\mv78200_amc_bsp"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\mv78200_amc_bsp"
            if [ "$CPU_BOARD" = ARM78200_BE ]; then
                TMP_OBJ_LIB_DIR_NAME="ARM78200_BE"
                BSP_NETWORK_DIR="${BSP_NETWORK_DIR}BE"
                DESTINATION_BSP_DIR="${DESTINATION_BSP_DIR}BE"
            fi
            ;;	
        ARM78200RD_tornado | ARM78200RD_BE_tornado)
            TMP_OBJ_LIB_DIR_NAME="ARM78200RD"
            CC_BSP_DIR="$DRIVE\\tornado\\target\\mv78200_amc_bsp\\config\\mv78200_amc_bsp"
            UNPACKED_BSP_DIR="$CPSS_PATH\\config\\mv78200_amc_bsp"    
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\mv78200rd_amc_bsp"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\mv78200rd_amc_bsp"
            if [ "$CPU_BOARD" = ARM78200RD_BE ]; then
                TMP_OBJ_LIB_DIR_NAME="ARM78200RD_BE"
                BSP_NETWORK_DIR="${BSP_NETWORK_DIR}BE"
                DESTINATION_BSP_DIR="${DESTINATION_BSP_DIR}BE"
            fi
            ;;				
        PPC860_DUNE_xxx)
            # TODO: unknown toolkit
            TMP_OBJ_LIB_DIR_NAME="PPC860"
            CC_BSP_DIR="$DRIVE\\tornado\\target\\dune_mpc860_bsp\\config"
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\dune_mpc860_bsp"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\dune_mpc860_bsp"
            ;;
        PPC860_NEGEV_xxx)
            # TODO: unknown toolkit???
            # BSP_NETWORK_DIR at non standard place: pss\sw\dune\bsp\...
            CC_BSP_DIR="$DRIVE\\pss\\SW\\dune\\bsp\\ppc860_negev\\config\\ppc860_negev"
            BSP_NETWORK_DIR="$CC_BSP_DIR"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\ppc860_negev"
            TMP_OBJ_LIB_DIR_NAME="PPC860"
            ;;
        *)
            error_message "BSP not declared: CPU_BOARD=$CPU_BOARD TOOLKIT=$TOOLKIT"
            return 1
    esac
    return 0
}

win32_marvell_BSP_copy()
{
    # copy_bsp
    special_makefile_location=no
    
    case $makefile_location_type in
            ARM5*81*)
                special_makefile_location=yes
                if win32_path_exists "$DESTINATION_BSP_DIR\\swControllers\\VxBsp\\makefile"
                then
                    already_copied=yes
                fi
                ;;
            ARM_EV | ARM_RD)
                if win32_path_exists "$DESTINATION_BSP_DIR\\BSP\\makefile"
                then
                    already_copied=yes
                fi
                ;;
            *)
                if win32_path_exists "$DESTINATION_BSP_DIR\\makefile"
                then
                    already_copied=yes
                fi
                ;;
    esac
        
    if [ "$already_copied" != "yes" ]; then
        check_create_dir "$DESTINATION_BSP_DIR"
        if [ "$special_makefile_location" != "yes" ]; then
            BSP_NETWORK_MAKE_DIR=$BSP_NETWORK_DIR
        fi    
        if win32_path_exists "$BSP_NETWORK_MAKE_DIR\\makefile" ; then
            info_message "****    Copy BSP from obj place   ****"
            info_message "xcopy $BSP_NETWORK_DIR\\*.*   $DESTINATION_BSP_DIR\\"
            xcopy "$BSP_NETWORK_DIR\\*.*"   "$DESTINATION_BSP_DIR\\"  /E /H /R /Y /Q
        else
            info_message "****    Copy BSP from ClearCase   ****"
            xcopy "$CC_BSP_DIR\\*.*"   "$DESTINATION_BSP_DIR\\"  /E /H /R /Y /Q
        fi
    fi

    return 0
}
