#!/bin/bash

mv_base_addr=0x800000001d020000

##########################################################
# Utility functions to be invoked
##########################################################

debug=0


debug_printf()
{
	if [ $debug -eq 1 ]
	then
		printf "$@"
	fi
}



read32_mv82210()
{
	local full_addr 
	local offset_addr
	offset_addr=$1
	echo -e "\n----------------------------------------"
	debug_printf "offset is 0x%llx\n" $offset_addr
	full_addr=$((mv_base_addr+offset_addr))
	debug_printf "read address is 0x%llx\n" ${full_addr}
	passarg=$(printf "0x%llx" ${full_addr})
	./bmutil -r -l -a $passarg -n 32

}

write32_mv82210()
{
	local full_addr 
	local offset_addr
	local write_value
	echo -e "\n"
	offset_addr=$1
	write_value=$2
	debug_printf "offset is 0x%llx\n" $offset_addr
	full_addr=$((mv_base_addr+offset_addr))
	debug_printf "write address is 0x%llx and value is 0x%0lx\n" ${full_addr} ${write_value}
	passarg=$(printf "0x%llx" ${full_addr})
	passarg2=$(printf "0x%lx" ${write_value})
	./bmutil -w -l -a $passarg -n 32 -v $passarg2
}

###################################################################
# Here we start the main loop
###################################################################
echo -e "Number of args ["$#"]"
#if [ $# =  1 ]
if [ $# -gt  0 ]
then
	if [ $1 = '-d' ] 
	then
		debug=1
		echo -e "debug is on\n"
	fi
fi

debug_printf "base is 0x%llx\n" $mv_base_addr

echo -e "\n\n######################################"
echo -e "Read the vendor id and device id"
echo -e "#####################################\n"
read32_mv82210 0x0000

echo -e "\n\n######################################"
echo -e "2.1 Enabling SPI-4 static alignment"
echo -e "#####################################\n"

write32_mv82210 0x00480 0x0
write32_mv82210 0x004B4 0x010000FF 
read32_mv82210 0x004C0

echo -e "\n\n######################################"
echo -e "2.2 Source calendar configuration"
echo -e "######################################\n"
write32_mv82210 0x100 0x0
write32_mv82210 0x10c 0x10000
write32_mv82210 0x10c 0x100000

######################################
echo -e "\n\n######################################"
echo -e "2.3 Sink calendar configuration"
echo -e "######################################\n"
write32_mv82210 0x100 0x0
write32_mv82210 0x110 0x10000
write32_mv82210 0x110 0x100000

#################################
echo -e "\n\n######################################"
echo -e "2.4 SPI-4 Max Burst Size"
echo -e "######################################\n"
write32_mv82210 0x118 0x5001F
write32_mv82210 0x114 0x00050005
write32_mv82210 0x8 0x0405431F

#################################
echo -e "\n\n######################################"
echo -e "2.5 Rate Limiter Threshold"
echo -e "######################################\n"
write32_mv82210 0x4008 0x1
write32_mv82210 0x8008 0x1

################################
echo -e "\n\n######################################"
echo -e "2.6 Receive FIFO Threshold"
echo -e "######################################\n"
write32_mv82210 0x80c 0x7f00780
write32_mv82210 0x00810 0x18001000
write32_mv82210 0x00814 0x07f007f0

###################################
echo -e "\n\n######################################"
echo -e "2.7 Transmit FIFO Threshold"
echo -e "######################################\n"
write32_mv82210 0x00800 0x32001900
write32_mv82210 0x00804 0x200
write32_mv82210 0x134 0xc00080

################################
echo -e "\n\n######################################"
echo -e "2.8 XG Port Configuration"
echo -e "######################################\n"
write32_mv82210 0x4000 0xB12401D0
write32_mv82210 0x8000 0xB12401D0
write32_mv82210 0x4010 0xD828000
write32_mv82210 0x8010 0xD828000
write32_mv82210 0x6000 0x501

###########################
echo -e "\n\n######################################"
echo -e "2.9 LED configuration"
echo -e "######################################\n"
write32_mv82210 0x603C 0x3DFE


###############################
echo -e "\n\n######################################"
echo -e "2.10 Device Global configuration"
echo -e "######################################\n"
write32_mv82210 0x4 0xB4600000

#############################
echo -e "\n\n######################################"
echo -e "2.11 Reserved Registers"
echo -e "######################################\n"
write32_mv82210 0x364 0x40C7
write32_mv82210 0xE900 0x80000000
write32_mv82210 0xF900 0x80000000
write32_mv82210 0x0600 0x33

#################################
echo -e "\n\n######################################"
echo -e "2.12 XAUI configuration"
echo -e "######################################\n"
write32_mv82210 0x6008 0xFF2c
write32_mv82210 0x6004 0x14A10277

##################################
echo -e "\n\n######################################"
echo -e "2.13 Checking XUAI interface"
echo -e "######################################\n"
write32_mv82210 0x6008 0x0
write32_mv82210 0x6004 0x1CA10000
read32_mv82210 0x6004
echo -e "##################Set finish########"
echo -e "Last return value bits[15:0] of offset 0x6004 should be 0x2040"

