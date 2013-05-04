# IP: Create a set 
0 ipset -N test iphash --hashsize 128 
# IP: Add first random value
0 ipset -A test 2.0.0.1
# IP: Add second random value
0 ipset -A test 192.168.68.69
# IP: Test first random value
0 ipset -T test 2.0.0.1
# IP: Test second random value
0 ipset -T test 192.168.68.69
# IP: Test value not added to the set
1 ipset -T test 2.0.0.2
# IP: Delete test set
0 ipset -X test
# IP: Restore values so that rehashing is triggered
0 ipset -R < iphash.t.restore
# IP: Check that all values are restored
0 (egrep -v '#|-N' iphash.t.restore | sort > .foo.1) && (ipset -S test | egrep -v '#|-N' | sort > .foo.2) && cmp .foo.1 .foo.2 && rm .foo.*
# IP: Delete test set
0 ipset -X test
# Network: Create a set 
0 ipset -N test iphash --hashsize 128 --netmask 24
# Network: Add first random network
0 ipset -A test 2.0.0.1
# Network: Add second random network
0 ipset -A test 192.168.68.69
# Network: Test first random value
0 ipset -T test 2.0.0.255
# Network: Test second random value
0 ipset -T test 192.168.68.95
# Network: Test value not added to the set
1 ipset -T test 2.0.1.0
# Network: Delete test set
0 ipset -X test
# eof
