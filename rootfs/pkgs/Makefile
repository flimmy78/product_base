
inst_netfilter:
	./instpkg.sh netfilter

inst_liblzo:
	./instpkg.sh liblzo

inst_openvpn: inst_liblzo
	./instpkg.sh openvpn

inst_pppox:
	./instpkg.sh pppox

inst_snort:
	./instpkg.sh snort

inst_utils:
	./instpkg.sh utils

inst_netutils:
	./instpkg.sh netutils

inst_mtrace:
	./instpkg.sh mtrace

inst_pureftpd:
	./instpkg.sh pure-ftpd

inst_ldap:
	./instpkg.sh ldap

inst_libc6:
	./instpkg.sh libc6

inst_debutils:
	./instpkg.sh debianutils

inst_tzdata:
	./instpkg.sh tzdata

inst_mrd6:
	./instpkg.sh mrd6

inst_ipsec:
	./instpkg.sh ipsec

inst_pamcrack:
	./instpkg.sh cracklibpam

cleanpkgs:
	-./removepkg.sh libc6 
	-./removepkg.sh netfilter
	-./removepkg.sh openvpn
	-./removepkg.sh pppox
	-./removepkg.sh snort
	-./removepkg.sh liblzo
	-./removepkg.sh netutils
	-./removepkg.sh utils
	-./removepkg.sh mtrace
	-./removepkg.sh pureftp
	-./removepkg.sh ldap
	-./removepkg.sh mrd6
	-./removepkg.sh ipsec
	-./removepkg.sh debianutils
	-./removepkg.sh tzdata
	-./removepkg.sh ipsec
	-./removepkg.sh cracklibpam

installleastpkgs:cleanpkgs
	make inst_libc6
	make inst_debutils

installpkgs: cleanpkgs
	make inst_libc6
	make inst_debutils
	make inst_tzdata
	make inst_netfilter
#	make inst_openvpn
	make inst_pppox
#	make inst_snort
	make inst_utils
	make inst_netutils
	make inst_pureftpd
#	make inst_ldap
	make inst_mrd6
	make inst_ipsec
	make inst_pamcrack
