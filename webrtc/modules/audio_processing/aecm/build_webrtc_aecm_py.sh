#!/bin/sh

# see https://pybind11.readthedocs.io/en/stable/compiling.html#building-manually

WORKSPACE_PATH=../../../../../..
INTERNAL_UTILS_PATH=$WORKSPACE_PATH/internal_utils
CPP_UTILS_PATH=$INTERNAL_UTILS_PATH/cpp_utils

WEBRTC_AUDIO_PROCESSING_PATH=../../../..

WEBRTC_AUDIO_PROCESSING_LIB=$WEBRTC_AUDIO_PROCESSING_PATH/webrtc/modules/audio_processing/.libs/libwebrtc_audio_processing.a

CWD=$(pwd)

echo Building webrtc-audio-processing package

cd $WEBRTC_AUDIO_PROCESSING_PATH

make

cd $CWD

echo
echo Building PyBind11 bindings "webrtc_aecm_py"
echo

CPP_OPTIONS="-std=c++17 -O3 -Wall"

CPP_SRC=webrtc_aecm_py.cpp

LIB_NAME=webrtc_aecm_py

OS="`uname`"

case $OS in
    'Linux')
        echo "Platform: Linux"
        LIB_OPT=-fPIC
        LIB_DIR="bin_linux_x64"
        ;;
    'Darwin')
        ARCH="`arch`"
        case $ARCH in
            'arm64')
                echo "Platform: Mac OS ARM"
                LIB_DIR="bin_mac_arm"
                ;;
            'i386')
                echo "Platform: Mac OS X64"
                LIB_DIR="bin_mac_x64"
                ;;
        esac
        LIB_OPT="-undefined dynamic_lookup"
        ;;
esac

LIB_EXT=$(python3-config --extension-suffix)

g++ $CPP_OPTIONS -I$CPP_UTILS_PATH -I$WEBRTC_AUDIO_PROCESSING_PATH -shared $LIB_OPT $(python3 -m pybind11 --includes) $WEBRTC_AUDIO_PROCESSING_LIB $CPP_SRC -o $LIB_NAME$LIB_EXT

LIB_EXPORT_DIR=$WORKSPACE_PATH/room_audio_sim/libs/webrtc/$LIB_DIR

echo Exporting $LIB_NAME$LIB_EXT to $LIB_EXPORT_DIR

mkdir -p $LIB_EXPORT_DIR

mv $LIB_NAME$LIB_EXT $LIB_EXPORT_DIR