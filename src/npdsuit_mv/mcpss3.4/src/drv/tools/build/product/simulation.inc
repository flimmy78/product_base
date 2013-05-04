# simulation settings
#
##############################################################################
# similation related environment variables
##############################################################################
#
# ASIC_SIMULATION - do we need to compile code that uses simulation
#                                                    -- empty / EXISTS
# RTOS_ON_SIM - do we use RTOS (Real Time Operating System) to connect to remote 
#               simulation                           -- empty / EXISTS
#
# DISTRIBUTED_SIMULATION_ROLE  - do we compile optimized code for distributed 
#                                application side / devices side
#                                                    -- APPLICATION_SIDE_ONLY /
#                                                       DEVICES_SIDE_ONLY /
#                                                       BROKER_ONLY /
#                                                       empty
#       NOTE : relevant when ASIC_SIMULATION is used
#
#

unset ASIC_SIMULATION
unset LINUX_SIM
unset RTOS_ON_SIM
unset DISTRIBUTED_SIMULATION_ROLE
case $SIMULATION in
    simulation | GM)
        export ASIC_SIMULATION=EXISTS
        if [ $TARGET_OS \!= win32 -a TARGET_OS \!= vxWorks ]
        then
            export LINUX_SIM=1
        fi
        if [ "$PRODUCT_TYPE" = "BROKER" ]
        then
            export DISTRIBUTED_SIMULATION_ROLE=BROKER_ONLY
        elif [ $TARGET_OS = Linux -o $TARGET_OS = FreeBSD ]
        then
            export RTOS_ON_SIM=EXISTS
            export DISTRIBUTED_SIMULATION_ROLE=APPLICATION_SIDE_ONLY
        fi
        case $FAMILY in
            DX_ALL | DXALL | DX_CHX | DX_CH3 | DX_CH3P )
                        if [ "$RTOS_ON_SIM" \!= "EXISTS" ] && [ "$TARGET_OS" = "win32" ]
                        then
                            export DRAGONITE_TYPE=A1
                        fi
            ;;
            *)            
            ;;            
        esac                        
        ;;
    HW | BM)
        ;;
    *)
        ;;
esac
