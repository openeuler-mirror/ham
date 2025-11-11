%global ham_version    1.0.0
%global type           libs

Name:          ubturbo-ham-%{type}
Version:       %{ham_version}
Release:       1
Summary:       Huawei HAM driver
License:       GPLv2
URL:           https://gitee.com/openeuler/ham
Source0:       ham-1.0.0.tar.gz
Provides:      %{name}
Vendor:        Huawei Technologies Co., Ltd.
BuildRoot:     %{buildroot}
ExclusiveArch: %arm64
BuildRequires: kernel-devel >= 5.10.0-136.12.0.86 make >= 4.3 gcc >= 10.3.1
Requires:      kernel >= 5.10.0-136.12.0.86

%define debug_package %{nil}
%define ham_module_dir /lib/modules/ham
%define ham_lib_dir /usr/lib64

%description
This package contains the Huawei HAM Driver

%prep
%setup -q -T -b 0 -c -n ham-1.0.0

%build
cd %{_builddir}/ham-1.0.0 && cmake -DCMAKE_BUILD_TYPE=release %{_builddir}/ham-1.0.0
make -j`nproc` install

%install
echo "########RPM_BUILD_ROOT=${RPM_BUILD_ROOT}"
rm -rf ${RPM_BUILD_ROOT}
mkdir -p -m755 ${RPM_BUILD_ROOT}/%{ham_module_dir}
mkdir -p -m755 ${RPM_BUILD_ROOT}/%{ham_lib_dir}
%{__install} -b -m 0644 %{_builddir}/ham-1.0.0/output/ham/lib/libham.so ${RPM_BUILD_ROOT}/%{ham_lib_dir}

%clean
rm -rf ${RPM_BUILD_ROOT}

%files
%defattr(-,root,root)
%{ham_lib_dir}/libham.so

%pre

%post

%preun

%postun
if [ "$1" = "0" ]; then
    rm -rf %{ham_module_dir}
    rm -f %{ham_lib_dir}/libham.so
fi
depmod -a

%changelog
