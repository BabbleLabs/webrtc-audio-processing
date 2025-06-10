//=============================================================================
//
//  aecm_py.cpp
//
//  Python Bindings for AECM module (see echo_control_mobile.h)
//
//=============================================================================
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

#include "dsp_tools.hpp"
#include "webrtc/modules/audio_processing/aecm/include/echo_control_mobile.h"

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

//-----------------------------------------------------------------------------

#define SAMPLE_RATE 16000   // [Hz]
#define FRAME_SIZE  160     // [samples]

//-----------------------------------------------------------------------------

class WEBRTC_AECM
{
public:
    WEBRTC_AECM(const int cng_mode = 1,
                const int echo_mode = 3)
    {
        // Create an AecMobile instance

        aecm_handle = WebRtcAecm_Create();

        // Initialize the AecMobile instance

        WebRtcAecm_Init(aecm_handle, SAMPLE_RATE);

        AecmConfig aecm_config = {int16_t(cng_mode), int16_t(echo_mode)};

        WebRtcAecm_set_config(aecm_handle, aecm_config);

        set_ECstartup(aecm_handle, 0);                      // This is needed to properly start the far end buffering
    }

    py::array_t<float> run(py::array_t<float>& near_end,
                           py::array_t<float>& far_end)
    {
        py::buffer_info near_end_info = near_end.request();
        py::buffer_info far_end_info = far_end.request();

        if (near_end_info.ndim !=1)
        {
            throw std::runtime_error("near_end must one 1 dimension");
        }

        if (far_end_info.ndim !=1)
        {
            throw std::runtime_error("far_end must one 1 dimension");
        }

        int num_samples = (std::min(near_end_info.size, far_end_info.size) / FRAME_SIZE) * FRAME_SIZE;

        auto output = py::array_t<float>(num_samples);
        py::buffer_info output_info = output.request();

        int16_t near_end_s16[FRAME_SIZE];
        int16_t far_end_s16[FRAME_SIZE];
        int16_t output_s16[FRAME_SIZE];

        for (int sample_idx = 0; sample_idx < num_samples; sample_idx += FRAME_SIZE)
        {
            real_to_int16(near_end_s16, static_cast<const float *>(near_end_info.ptr) + sample_idx, FRAME_SIZE);
            real_to_int16(far_end_s16, static_cast<const float *>(far_end_info.ptr) + sample_idx, FRAME_SIZE);

            WebRtcAecm_BufferFarend(aecm_handle, far_end_s16, FRAME_SIZE);              // Feed AECM far end input with one frame

            WebRtcAecm_Process(aecm_handle,                                         // Process near end input
                            near_end_s16,
                            NULL,
                            output_s16,
                            FRAME_SIZE,
                            0);                      // bulk delay set to zero

            int16_to_real(static_cast<float *>(output_info.ptr) + sample_idx, output_s16, FRAME_SIZE);
        }

        return output;
    }

private:
    void* aecm_handle = NULL;
};


//-----------------------------------------------------------------------------
//  Python Bindinds
//-----------------------------------------------------------------------------

PYBIND11_MODULE(webrtc_aecm_py, m) {
    m.doc() = "WEBRTC AECM module Python bindings";

    py::class_<WEBRTC_AECM>(m, "WEBRTC_AECM")
        .def(py::init<const int, const int>(), py::arg("cng_mode") = 1, py::arg("echo_mode") = 3)

        .def("__call__", &WEBRTC_AECM::run, "WEBRTC AECM processing", py::arg("near_end"), py::arg("far_end"));
}

//=============================================================================
