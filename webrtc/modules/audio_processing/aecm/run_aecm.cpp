//=============================================================================
//
//  run_aecm.cpp
//
//  echo_control_mobile.c test program
//
//  Copyright (c) 2021 by Cisco, Inc.  ALL RIGHTS RESERVED.
//  These coded instructions, statements, and computer programs are the
//  copyrighted works and confidential proprietary information of BabbleLabs, Inc.
//  They may not be modified, copied, reproduced, distributed, or disclosed to
//  third parties in any manner, medium, or form, in whole or in part, without
//  the prior written consent of Cisco, Inc.
//
//  $Author: ansantil@cisco.com
//
//=============================================================================

#include <iostream>
#include <cmath>
#include <cxxopts.hpp>

#include "dsp_tools.hpp"

#include "webrtc/modules/audio_processing/aecm/include/echo_control_mobile.h"
#include "webrtc/common_audio/wav_file.h"

//=============================================================================

#define SAMPLE_RATE 16000   // [Hz]
#define FRAME_SIZE  160     // [samples]

//=============================================================================

int main(int argc, char **argv)
{
    // Parse command line arguments

    std::cout << std::endl;

    std::string program_description = "WebRTC echo_control_mobile test program\n";

    cxxopts::Options options("run_aecm", program_description);

    options.add_options()
        ("near",     "near end input WAV file name", cxxopts::value<std::string>())
        ("far",      "far end input WAV file name",  cxxopts::value<std::string>())
        ("output",   "output WAV file name",         cxxopts::value<std::string>())
        ("help",     "Usage help");

    options.parse_positional({"near", "far", "output"});

    options.show_positional_help();

    auto result = options.parse(argc, argv);

    if (argc == 1 || result.count("help"))
    {
        std::cout << options.help() << std::endl;

        exit(0);
    }

    auto unmatched_results = result.unmatched();

    if (!unmatched_results.empty())
    {
        for (int idx = 0; idx < unmatched_results.size(); idx++)
        {
            std::cout << "Unsupported option: " << unmatched_results[idx] << std::endl;
        }
    }

    std::string near_end_file_name;
    std::string far_end_file_name;
    std::string output_file_name;

    if (result.count("near"))
        near_end_file_name = result["near"].as<std::string>();

    if (result.count("far"))
        far_end_file_name = result["far"].as<std::string>();

    if (result.count("output"))
        output_file_name = result["output"].as<std::string>();


    // Load near end and far end input files

    webrtc::WavReader near_end_file(near_end_file_name);

    if (near_end_file.sample_rate() != SAMPLE_RATE)
    {
        std::cerr << near_end_file_name << " sample rate mismatch" << std::endl;
    }

    webrtc::WavReader far_end_file(far_end_file_name);

    if (far_end_file.sample_rate() != SAMPLE_RATE)
    {
        std::cerr << far_end_file_name << " sample rate mismatch" << std::endl;
    }


    // Create output audio files

    const int num_samples = std::min(near_end_file.num_samples(), far_end_file.num_samples());

    webrtc::WavWriter output_file(output_file_name, SAMPLE_RATE, 1);


    // Create an AecMobile instance

    void* aecm_handle = WebRtcAecm_Create();

    if (WebRtcAecm_Init(aecm_handle, SAMPLE_RATE))
    {
        std::cerr << "WebRtcAecm_Init() failed" << std::endl;
    }

    set_ECstartup(aecm_handle, 0);          // This is needed to properly start the far end buffering

    std::cout << "ECstartup = " << get_ECstartup(aecm_handle) << "\n" << std::endl;


    // Frame processing

    std::cout << "Start frame processing \n" << std::endl;

    for (int sample_idx = 0; sample_idx < num_samples; sample_idx += FRAME_SIZE)
    {
        int16_t near_end[FRAME_SIZE];
        int16_t far_end[FRAME_SIZE];
		int16_t output[FRAME_SIZE];

        near_end_file.ReadSamples(FRAME_SIZE, near_end);
        far_end_file.ReadSamples(FRAME_SIZE, far_end);

        WebRtcAecm_BufferFarend(aecm_handle, far_end, FRAME_SIZE);              // Feed AECM far end input with one frame

        WebRtcAecm_Process(aecm_handle,                                         // Process near end input
                           near_end,
                           NULL,
                           output,
                           FRAME_SIZE,
                           0);                      // bulk delay set to zero

        output_file.WriteSamples(output, FRAME_SIZE);
    }

    std::cout << "Frame processing completed\n" << std::endl;

    // Destroy the AecMobile instance

    WebRtcAecm_Free(aecm_handle);
}

//=============================================================================
