#!/bin/bash

dbus-send --system --dest=aw.npd --type=method_call --print-reply=literal /aw/npd aw.npd.show_ver
echo
dbus-send --system --dest=aw.npd --type=method_call --print-reply=literal /aw/npd aw.npd.show_hwconf
echo
dbus-send --system --dest=aw.npd --type=method_call --print-reply=literal /aw/ethports aw.ethports.show_ethport_list
echo
dbus-send --system --dest=aw.npd --type=method_call --print-reply=literal /aw/ethports aw.ethports.show_ethport_one byte:1 byte:4
