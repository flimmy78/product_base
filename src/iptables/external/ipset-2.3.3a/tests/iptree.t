# Static: Create a set without timeout
0 ipset -N test iptree
# Static: Add first random entry
0 ipset -A test 2.0.0.1
# Static: Add second random value
0 ipset -A test 192.168.68.69
# Static: Test first random value
0 ipset -T test 2.0.0.1
# Static: Test second random value
0 ipset -T test 192.168.68.69
# Static: Test value not added to the set
1 ipset -T test 2.0.0.2
# Static: Test value not added to the set
1 ipset -T test 192.168.68.70
# Static: Delete test test
0 ipset -X test
# Timeout: Create a set with a timeout parameter
0 ipset -N test iptree --timeout 5
# Timeout: Add first random entry
0 ipset -A test 2.0.0.1
# Timeout: Add second random value
0 ipset -A test 192.168.68.69
# Timeout: Test first random value
0 ipset -T test 2.0.0.1
# Timeout: Test second random value
0 ipset -T test 192.168.68.69
# Timeout: Test value not added to the set
1 ipset -T test 2.0.0.2
# Timeout: Test value not added to the set
1 ipset -T test 192.168.68.70
# Timeout: Sleep 6s so that entries can time out
0 sleep 6
# Timeout: Test first random value
1 ipset -T test 2.0.0.1
# Timeout: Test second random value
1 ipset -T test 192.168.68.69
# Timeout: Test value not added to the set
1 ipset -T test 2.0.0.2
# Timeout: Test value not added to the set
1 ipset -T test 192.168.68.70
# Timeout: Delete test set
0 ipset -X test
# eof
