#!/bin/sh

EXTERNAL_UTILS_PATH=../../../..
INTERNAL_UTILS_PATH=../../../../../internal_utils

CXXOPTS_INC_DIR=$EXTERNAL_UTILS_PATH/cxxopts/include
AUDIOFILE_INC_DIR=$EXTERNAL_UTILS_PATH/AudioFile
CPP_UTILS_PATH=$INTERNAL_UTILS_PATH/cpp_utils

WEBRTC_AUDIO_PROCESSING_PATH=../../..

WEBRTC_AUDIO_PROCESSING_LIB=$WEBRTC_AUDIO_PROCESSING_PATH/webrtc/modules/audio_processing/.libs/libwebrtc_audio_processing.a

CWD=$(pwd)

cd $WEBRTC_AUDIO_PROCESSING_PATH

make

cd $CWD

echo
echo building run_webrtc_vad

g++ -std=c++17 -O3 -Wall -I$CXXOPTS_INC_DIR -I$AUDIOFILE_INC_DIR -I$CPP_UTILS_PATH -I$WEBRTC_AUDIO_PROCESSING_PATH run_webrtc_vad.cpp $WEBRTC_AUDIO_PROCESSING_LIB -o run_webrtc_vad
