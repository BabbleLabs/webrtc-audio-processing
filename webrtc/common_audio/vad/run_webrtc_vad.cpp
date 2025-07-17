//=============================================================================
//
//  run_webrtc_vad.cpp
//
//  webrtc_vad.c test program
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

#include "webrtc/common_audio/vad/include/webrtc_vad.h"
#include "webrtc/common_audio/wav_file.h"

//=============================================================================

#define SAMPLE_RATE 16000   // [Hz]
#define FRAME_SIZE  160     // [samples]

//=============================================================================

int main(int argc, char **argv)
{
    // Parse command line arguments

    std::cout << std::endl;

    std::string program_description = "WebRTC VAD test program\n";

    cxxopts::Options options("run_aecm", program_description);

    options.add_options()
        ("input",    "input WAV file name", cxxopts::value<std::string>())
        ("output",   "output WAV file name", cxxopts::value<std::string>())
        ("m, mode",  "mode (either 0, 1, 2, 3) ", cxxopts::value<int>()->default_value("0"))
        ("help",     "Usage help");

    options.parse_positional({"input", "output"});

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
            std::cerr << "Unsupported option: " << unmatched_results[idx] << std::endl;
        }
    }

    std::string input_file_name;
    std::string output_file_name;

    if (result.count("input"))
        input_file_name = result["input"].as<std::string>();

    if (result.count("output"))
        output_file_name = result["output"].as<std::string>();


    // Load near end and far end input files

    webrtc::WavReader input_file(input_file_name);

    if (input_file.sample_rate() != SAMPLE_RATE)
    {
        std::cerr << input_file_name << " sample rate mismatch" << std::endl;
    }


    // Create output audio files

    const int num_samples = input_file.num_samples();

    webrtc::WavWriter output_file(output_file_name, SAMPLE_RATE, 1);


    // Create a VAD instance

    VadInst* vad_handle = WebRtcVad_Create();

    if (WebRtcVad_Init(vad_handle))
    {
        std::cerr << "WebRtcVad_Init() failed" << std::endl;
    }

    int mode = result["mode"].as<int>();

    std::cout << "mode = " << mode << "\n" << std::endl;

    WebRtcVad_set_mode(vad_handle, mode);


    // Frame processing

    std::cout << "Start frame processing \n" << std::endl;

    for (int sample_idx = 0; sample_idx < num_samples; sample_idx += FRAME_SIZE)
    {
        int16_t input[FRAME_SIZE];
		int16_t output[FRAME_SIZE];

        input_file.ReadSamples(FRAME_SIZE, input);

        bool vad_flag = (WebRtcVad_Process(vad_handle, SAMPLE_RATE, input, FRAME_SIZE) == 1);

        if (vad_flag)
        {
            copy_array(output, input, FRAME_SIZE);
        }
        else
        {
            clear_array(output, FRAME_SIZE);
        }

        output_file.WriteSamples(output, FRAME_SIZE);
    }

    std::cout << "Frame processing completed\n" << std::endl;

    // Destroy the VAD instance

// DEBUG    WebRtcVad_Free(vad_handle);
}

//=============================================================================
