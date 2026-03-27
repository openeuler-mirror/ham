#!/bin/bash

set -e

git submodule update --init --recursive

patch_mockcpp()
{
  if [ -f $CURRENT_PATH/3rdparty/mockcpp/mockcpp_support_arm64.patch ]; then
    return 0
  fi
  cp -r $CURRENT_PATH/3rdparty/mockcpp_support_arm64.patch $CURRENT_PATH/3rdparty/mockcpp
  cd $CURRENT_PATH/3rdparty/mockcpp
  dos2unix src/UnixCodeModifier.cpp
  git apply mockcpp_support_arm64.patch
}

CURRENT_PATH=$(dirname "$(readlink -f "$0")")
echo "${CURRENT_PATH}"
cd ${CURRENT_PATH}
code_dir=$(dirname "${CURRENT_PATH}")

patch_mockcpp

BUILD_DIR=$CURRENT_PATH/build
rm -rf ${BUILD_DIR}
mkdir -p ${BUILD_DIR}

N_CPUS=$(grep processor /proc/cpuinfo | wc -l)
echo "$N_CPUS processors detected."

cd ${BUILD_DIR}
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j $((N_CPUS-2))
if [ $? -ne 0 ]; then
	  echo "Failed to configure ham_dt build!"
    exit 1
fi

echo Success
mkdir -p gcovr_report
./ham_dt --gtest_output=xml:gcovr_report/test_detail.xml

lcov --d ./ --c --output-file test.info --rc lcov_branch_coverage=1
lcov -e test.info "*${code_dir}/src/*" -output-file coverage.info --rc lcov_branch_coverage=1
genhtml -o gcovr_report coverage.info --show-details --legend --rc lcov_branch_coverage=1