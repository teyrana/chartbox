#!/bin/bash

INVOCATION_ABS_DIR=`pwd`
BUILD_TYPE="Release"
BUILD_DIR="build"

pushd $BUILD_DIR

bin/testall "$@"

SUCCESS=$?
if [ "$SUCCESS" -ne "0" ]; then
    exit $SUCCESS
fi


popd
