# This spec file was generated using Kpp
# If you find any problems with this spec file please report
# the error to ian geiser <geiseri@msoe.edu>
Summary:   KDE application to manage Alcatel 50x and 70x phone
Name:      kalcatel
Version:   0.3
Release:   1
Copyright: GPL
Vendor:    Michal Cihar <cihar@email.cz>
Url:       http://cihar.liten.cz
Packager:  Michal Cihar <cihar@email.cz>
Group:     Applications
Source:    kalcatel-0.3.tar.gz
BuildRoot: /var/tmp/%{name}-%{version}-%{release}-root

%description
This application allows you to manage calls, messages, contacts,
todos and calendar in your Alcatel One Touch 50x and 70x mobile.

Protocol used by this phone is binary and not documented
so this program could not guarantee any functionality in your case
but it works well for me.

%prep
%setup
CFLAGS="$RPM_OPT_FLAGS" CXXFLAGS="$RPM_OPT_FLAGS" ./configure \
                 --prefix=/usr \
                $LOCALFLAGS
%build
# Setup for parallel builds
numprocs=`egrep -c ^cpu[0-9]+ /proc/stat || :`
if [ "$numprocs" = "0" ]; then
  numprocs=1
fi

make -j$numprocs

%install
make install-strip DESTDIR=$RPM_BUILD_ROOT

cd $RPM_BUILD_ROOT
gzip -9 usr/man/man1/kalcatel.1
find . -type d | sed '1,2d;s,^\.,\%attr(-\,root\,root) \%dir ,' > $RPM_BUILD_DIR/file.list.kalcatel
find . -type f | sed 's,^\.,\%attr(-\,root\,root) ,' >> $RPM_BUILD_DIR/file.list.kalcatel
find . -type l | sed 's,^\.,\%attr(-\,root\,root) ,' >> $RPM_BUILD_DIR/file.list.kalcatel

%clean
rm -rf $RPM_BUILD_ROOT/*
rm -rf $RPM_BUILD_DIR/kalcatel
rm -rf ../file.list.kalcatel


%files -f ../file.list.kalcatel
