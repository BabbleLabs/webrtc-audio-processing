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

#include <AudioFile.h>

#include "dsp_tools.hpp"

#include "webrtc/modules/audio_processing/aecm/include/echo_control_mobile.h"

//=============================================================================

#define SAMPLE_RATE 16000   // [Hz]
#define FRAME_SIZE  160     // [samples]

//=============================================================================

class WaveFile : public AudioFile<int16_t>
{
public:
    WaveFile() {}

    WaveFile(int sampleRate, int numSamples, int numChannels = 1)
    {
        setNumChannels(numChannels);            // Set the number of channels
        setBitDepth(16);                        // 16 bits signed integer
        setSampleRate(sampleRate);              // Set the sample rate
        setNumSamplesPerChannel(numSamples);    // Set the number of samples per channel
    }

    void load(std::string filePath,             // Input WAV file full path
              int sample_rate = 0)              // Default is do not check the sampling rate
    {
        if (AudioFile<int16_t>::load(filePath))
        {
            std::cout << "Input File: " << filePath << std::endl;
            std::cout << "Bit Depth: " << getBitDepth() << std::endl;
            std::cout << "Sample Rate: " << getSampleRate() << std::endl;
            std::cout << "Num Channels: " << getNumChannels() << std::endl;
            std::cout << "Length in Seconds: " << getLengthInSeconds() << std::endl;
            std::cout << std::endl;

            if (getNumChannels() != 1)
            {
                throw std::runtime_error("input file ''" + filePath + "'' number of channel mismatch");
            }

            if (sample_rate > 0 && int(getSampleRate()) != sample_rate)
            {
                throw std::runtime_error("input file ''" + filePath + "'' sampling rate mismatch");
            }
        }
        else
        {
            throw std::runtime_error("Unable to read input file " + filePath);
        }
    }

    bool save(std::string filePath)             // Output WAV file full path
    {
        std::cout << "Output File: " << filePath << std::endl;
        std::cout << "Bit Depth: " << getBitDepth() << std::endl;
        std::cout << "Sample Rate: " << getSampleRate() << std::endl;
        std::cout << "Num Channels: " << getNumChannels() << std::endl;
        std::cout << "Length in Seconds: " << getLengthInSeconds() << std::endl;
        std::cout << std::endl;

        return AudioFile<int16_t>::save(filePath, AudioFileFormat::Wave);
    }
};

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

    WaveFile near_end_file;
    WaveFile far_end_file;

    near_end_file.load(near_end_file_name, SAMPLE_RATE);
    far_end_file.load(far_end_file_name, SAMPLE_RATE);


    // Create output audio files

    const int num_samples = near_end_file.getNumSamplesPerChannel();

    WaveFile output_file(SAMPLE_RATE, num_samples);


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
        int16_t* near_end = (int16_t *)&near_end_file.samples[0][sample_idx];
        int16_t* far_end = (int16_t *)&far_end_file.samples[0][sample_idx];
		int16_t* output = (int16_t *)&output_file.samples[0][sample_idx];

        WebRtcAecm_BufferFarend(aecm_handle, far_end, FRAME_SIZE);              // Feed AECM far end input with one frame

        WebRtcAecm_Process(aecm_handle,                                         // Process near end input
                           near_end,
                           NULL,
                           output,
                           FRAME_SIZE,
                           0);                      // bulk delay set to zero
    }

    std::cout << "Frame processing completed\n" << std::endl;


    // Save output audio files

    output_file.save(output_file_name);


    // Destroy the AecMobile instance

    WebRtcAecm_Free(aecm_handle);
}

//=============================================================================
