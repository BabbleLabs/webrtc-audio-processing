#!/bin/sh

# see https://pybind11.readthedocs.io/en/stable/compiling.html#building-manually

INTERNAL_UTILS_PATH=../../../../../../internal_utils
CPP_UTILS_PATH=$INTERNAL_UTILS_PATH/cpp_utils

WEBRTC_AUDIO_PROCESSING_PATH=../../../..

WEBRTC_AUDIO_PROCESSING_LIB=$WEBRTC_AUDIO_PROCESSING_PATH/webrtc/modules/audio_processing/.libs/libwebrtc_audio_processing.a

CPP_OPTIONS="-std=c++17 -O3 -Wall"

CPP_SRC=webrtc_aecm_py.cpp

LIB_NAME=webrtc_aecm_py

OS="`uname`"

case $OS in
    'Linux')
        echo "Platform: Linux"
        LIB_OPT=-fPIC
        ;;
    'Darwin')
        ARCH="`arch`"
        case $ARCH in
            'arm64')
                echo "Platform: Mac OS ARM"
                ;;
            'i386')
                echo "Platform: Mac OS X64"
                ;;
        esac
        LIB_OPT="-undefined dynamic_lookup"
        ;;
esac


g++ $CPP_OPTIONS -I$CPP_UTILS_PATH -I$WEBRTC_AUDIO_PROCESSING_PATH -shared $LIB_OPT $(python3 -m pybind11 --includes) $WEBRTC_AUDIO_PROCESSING_LIB $CPP_SRC -o $LIB_NAME$(python3-config --extension-suffix)
