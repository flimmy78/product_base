#
# Makefile for the Ethernet Trunking driver
#

obj-$(CONFIG_TRUNKING) += trunking.o

trunking-objs := trunk_main.o trunk_3ad.o trunk_alb.o trunk_sysfs.o

ipv6-$(subst m,y,$(CONFIG_IPV6)) += trunk_ipv6.o
trunking-objs += $(ipv6-y)

