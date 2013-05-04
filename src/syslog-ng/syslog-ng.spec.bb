#
#  Specfile used by BalaBit internally.
#
Summary: Next generation system logging daemon
Name: syslog-ng
Version: 2.0.0
Release: 1
License: GPL
Group: System Environment/Daemons
Source: syslog-ng_%{version}.tar.gz
URL: http://www.balabit.com
Packager: Sandor Geller <wildy@balabit.com>
Vendor: Balabit IT Ltd.
BuildRoot: %{_tmppath}/%{name}-root
BuildRequires: bison, flex, gcc-c++
#BuildConflicts:
#Exclusivearch: i386

%define prefix /

%description
 Syslog-ng is a next generation system logger daemon which provides more
 capabilities and is has a more flexible configuration then the traditional
 syslog daemon.

%prep
%setup -q -n syslog-ng-%{version}

%build

# build syslog-ng using the bundled libol

./configure --prefix=/ --mandir=/usr/share/man --infodir=/usr/share/info \
  --sysconfdir=/etc
make

%install
make install DESTDIR="$RPM_BUILD_ROOT"
# strip the binaries/ libraries
strip ${RPM_BUILD_ROOT}/sbin/syslog-ng
#if [ -r /etc/redhat-release ]; then
#	if grep -q "Red Hat Enterprise Linux ES" /etc/redhat-release; then
		install -D -o root -g root -m 0644 contrib/rhel-packaging/syslog-ng.conf \
		  ${RPM_BUILD_ROOT}/etc/syslog-ng/syslog-ng.conf
		install -D -o root -g root -m 0755 contrib/rhel-packaging/syslog-ng.init \
		  ${RPM_BUILD_ROOT}/etc/rc.d/init.d/syslog-ng
		install -D -o root -g root -m 0644 contrib/rhel-packaging/syslog-ng.logrotate \
		  ${RPM_BUILD_ROOT}/etc/logrotate.d/syslog-ng
#	fi
#fi
# install documentation
install -D -o root -g root -m 0644 doc/reference/syslog-ng.html.tar.gz \
  ${RPM_BUILD_ROOT}/usr/share/doc/syslog-ng-2.0.0/syslog-ng.html.tar.gz
install -D -o root -g root -m 0644 doc/reference/syslog-ng.txt \
  ${RPM_BUILD_ROOT}/usr/share/doc/syslog-ng-2.0.0/syslog-ng.txt
install -D -o root -g root -m 0644 ChangeLog \
  ${RPM_BUILD_ROOT}/usr/share/doc/syslog-ng-2.0.0/ChangeLog
install -D -o root -g root -m 0644 NEWS \
  ${RPM_BUILD_ROOT}/usr/share/doc/syslog-ng-2.0.0/NEWS
install -D -o root -g root -m 0644 README \
  ${RPM_BUILD_ROOT}/usr/share/doc/syslog-ng-2.0.0/README
install -D -o root -g root -m 0644 AUTHORS \
  ${RPM_BUILD_ROOT}/usr/share/doc/syslog-ng-2.0.0/AUTHORS
install -D -o root -g root -m 0644 COPYING \
  ${RPM_BUILD_ROOT}/usr/share/doc/syslog-ng-2.0.0/COPYING

%files
%defattr(-,root,root)
/sbin/syslog-ng
/etc/rc.d/init.d/syslog-ng
/usr/share/man/*
%docdir /usr/share/doc/syslog-ng-2.0.0
/usr/share/doc/syslog-ng-2.0.0/*
%config /etc/syslog-ng/syslog-ng.conf
%config /etc/logrotate.d/syslog-ng

%post
chkconfig --add syslog-ng
sh /etc/rc.d/init.d/syslog-ng start || exit 0

%preun
sh /etc/rc.d/init.d/syslog-ng stop || exit 0

%postun
chkconfig --del syslog-ng

%check
%clean
[ $RPM_BUILD_ROOT == / ] || rm -rf $RPM_BUILD_ROOT

%changelog
* Fri Jun 30 2006 Tamas Pal <folti@balabit.com>
- fixed typo in RHEL config file.
* Mon Mar 27 2006 Balazs Scheidler <bazsi@balabit.com>
- removed postscript version of the documentation
* Fri Sep 9 2005 Sandor Geller <wildy@balabit.com>
- fixed permissions of /etc/rc.d/init.d/syslog-ng
* Thu Jun 30 2005 Sandor Geller <wildy@balabit.com>
- packaging fixes, added logrotate script
* Thu Jun 23 2005 Sandor Geller <wildy@balabit.com>
- added upstream's documentation to the package
* Mon Jun 20 2005 Sandor Geller <wildy@balabit.com>
- initial RPM packaging for RHEL ES
