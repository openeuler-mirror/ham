#!/bin/bash
# Copyright: (c) Huawei Technologies Co., Ltd. 2024. All rights reserved.

set -e

usage() {
    echo "Usage: $0 [ -h | -help ] [ -t | -type <build_type> ] [--cli=Diagnose] [--ut=UT] [--tp=tracepoint]"
    echo "build_type: [debug, release, clean]"
    echo "Examples:"
    echo " 1 ./build.sh -t release "
    echo " 2 ./build.sh -t debug "
    echo " 3 ./build.sh -t debug [--ut] // 限制仅DT构建脚本使用"
    echo
    exit 1;
}

PROJ_DIR="$(realpath "$(dirname "${BASH_SOURCE[0]}")")"
BUILD_DIR=${PROJ_DIR}/build

rm -rf ${BUILD_DIR} && mkdir -p ${BUILD_DIR}

clean()
{
    cd $BUILD_DIR
    if make clean; then
        rm -rf ${PROJ_DIR}/output
        exit 0
    else
        echo "Failed to clean ham."
        exit 1
    fi
}

pack()
{
    cd ${PROJ_DIR}/output
    rm -rf ham_package
    mv ham ham_package
    tar -czvf ham_1.0.0_$(uname -s)-$(uname -m)_${1}.tar.gz ham_package
    exit 0
}

cmake_build()
{
    CPU_PROCESSOR_NUM=$(grep processor /proc/cpuinfo | wc -l)
    if cmake -DCMAKE_BUILD_TYPE=$1 $CMAKE_FLAGS $PROJ_DIR; then
        if make install -j ${CPU_PROCESSOR_NUM}; then
            echo "Build ham successfully, start packing."
            pack "$1"
        else
            echo "Build ham error."
            exit 1
        fi
    else
        echo "Cmake cmd error."
        exit 1
    fi
}

while true; do
    case "$1" in
        -t | --type )
            type=${2,,}
            if [ $type = "clean" ]; then
                clean
            elif [ $type = "release" ] || [ $type = "debug" ]; then
                shift 2
                continue
            else
                echo "Invalid build type $2"
                usage
                exit 1
            fi
            shift 2
            ;;
		    --ut )
			      CMAKE_FLAGS+="-DDEBUG_UT"
            shift ;;
		    -h | -help )
            usage
            exit 0
            ;;
        * )
            break;;
    esac
done

cmake_build "$type"
