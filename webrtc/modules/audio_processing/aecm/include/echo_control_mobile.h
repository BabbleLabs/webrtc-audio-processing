/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MODULES_AUDIO_PROCESSING_AECM_INCLUDE_ECHO_CONTROL_MOBILE_H_
#define WEBRTC_MODULES_AUDIO_PROCESSING_AECM_INCLUDE_ECHO_CONTROL_MOBILE_H_

#include <stdlib.h>

#include "webrtc/typedefs.h"

enum {
    AecmFalse = 0,
    AecmTrue
};

// Errors
#define AECM_UNSPECIFIED_ERROR           12000
#define AECM_UNSUPPORTED_FUNCTION_ERROR  12001
#define AECM_UNINITIALIZED_ERROR         12002
#define AECM_NULL_POINTER_ERROR          12003
#define AECM_BAD_PARAMETER_ERROR         12004

// Warnings
#define AECM_BAD_PARAMETER_WARNING       12100

typedef struct {
    int16_t cngMode;            // AECM_FALSE, AECM_TRUE (default)
    int16_t echoMode;           // 0, 1, 2, 3 (default), 4
} AecmConfig;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Allocates the memory needed by the AECM. The memory needs to be
 * initialized separately using the WebRtcAecm_Init() function.
 * Returns a pointer to the instance and a nullptr at failure.
 */
void* WebRtcAecm_Create();

/*
 * This function releases the memory allocated by WebRtcAecm_Create()
 *
 * Inputs                       Description
 * -------------------------------------------------------------------
 * void*    aecmInst            Pointer to the AECM instance
 */
void WebRtcAecm_Free(void* aecmInst);

/*
 * Initializes an AECM instance.
 *
 * Inputs                       Description
 * -------------------------------------------------------------------
 * void*          aecmInst      Pointer to the AECM instance
 * int32_t        sampFreq      Sampling frequency of data
 *
 * Outputs                      Description
 * -------------------------------------------------------------------
 * int32_t        return        0: OK
 *                             -1: error
 */
int32_t WebRtcAecm_Init(void* aecmInst, int32_t sampFreq);

/*
 * Inserts an 80 or 160 sample block of data into the farend buffer.
 *
 * Inputs                       Description
 * -------------------------------------------------------------------
 * void*          aecmInst      Pointer to the AECM instance
 * int16_t*       farend        In buffer containing one frame of
 *                              farend signal
 * int16_t        nrOfSamples   Number of samples in farend buffer
 *
 * Outputs                      Description
 * -------------------------------------------------------------------
 * int32_t        return        0: OK
 *                             -1: error
 */
int32_t WebRtcAecm_BufferFarend(void* aecmInst,
                                const int16_t* farend,
                                size_t nrOfSamples);

/*
 * Runs the AECM on an 80 or 160 sample blocks of data.
 *
 * Inputs                        Description
 * -------------------------------------------------------------------
 * void*          aecmInst       Pointer to the AECM instance
 * int16_t*       nearendNoisy   In buffer containing one frame of
 *                               reference nearend+echo signal. If
 *                               noise reduction is active, provide
 *                               the noisy signal here.
 * int16_t*       nearendClean   In buffer containing one frame of
 *                               nearend+echo signal. If noise
 *                               reduction is active, provide the
 *                               clean signal here. Otherwise pass a
 *                               NULL pointer.
 * int16_t        nrOfSamples    Number of samples in nearend buffer
 * int16_t        msInSndCardBuf Delay estimate for sound card and
 *                               system buffers
 *
 * Outputs                       Description
 * -------------------------------------------------------------------
 * int16_t*       out            Out buffer, one frame of processed nearend
 * int32_t        return         0: OK
 *                              -1: error
 */
int32_t WebRtcAecm_Process(void* aecmInst,
                           const int16_t* nearendNoisy,
                           const int16_t* nearendClean,
                           int16_t* out,
                           size_t nrOfSamples,
                           int16_t msInSndCardBuf);

/*
 * This function enables the user to set certain parameters on-the-fly
 *
 * Inputs                       Description
 * -------------------------------------------------------------------
 * void*          aecmInst      Pointer to the AECM instance
 * AecmConfig     config        Config instance that contains all
 *                              properties to be set
 *
 * Outputs                      Description
 * -------------------------------------------------------------------
 * int32_t        return        0: OK
 *                             -1: error
 */
int32_t WebRtcAecm_set_config(void* aecmInst, AecmConfig config);

/*
 * This function enables the user to set certain parameters on-the-fly
 *
 * Inputs                       Description
 * -------------------------------------------------------------------
 * void*          aecmInst      Pointer to the AECM instance
 *
 * Outputs                      Description
 * -------------------------------------------------------------------
 * AecmConfig*    config        Pointer to the config instance that
 *                              all properties will be written to
 * int32_t        return        0: OK
 *                             -1: error
 */
int32_t WebRtcAecm_get_config(void *aecmInst, AecmConfig *config);

/*
 * This function enables the user to set the echo path on-the-fly.
 *
 * Inputs                       Description
 * -------------------------------------------------------------------
 * void*        aecmInst        Pointer to the AECM instance
 * void*        echo_path       Pointer to the echo path to be set
 * size_t       size_bytes      Size in bytes of the echo path
 *
 * Outputs                      Description
 * -------------------------------------------------------------------
 * int32_t      return          0: OK
 *                             -1: error
 */
int32_t WebRtcAecm_InitEchoPath(void* aecmInst,
                                const void* echo_path,
                                size_t size_bytes);

/*
 * This function enables the user to get the currently used echo path
 * on-the-fly
 *
 * Inputs                       Description
 * -------------------------------------------------------------------
 * void*        aecmInst        Pointer to the AECM instance
 * void*        echo_path       Pointer to echo path
 * size_t       size_bytes      Size in bytes of the echo path
 *
 * Outputs                      Description
 * -------------------------------------------------------------------
 * int32_t      return          0: OK
 *                             -1: error
 */
int32_t WebRtcAecm_GetEchoPath(void* aecmInst,
                               void* echo_path,
                               size_t size_bytes);

/*
 * This function enables the user to get the echo path size in bytes
 *
 * Outputs                      Description
 * -------------------------------------------------------------------
 * size_t       return          Size in bytes
 */
size_t WebRtcAecm_echo_path_size_bytes();

/*
 * Gets the last error code.
 *
 * Inputs                       Description
 * -------------------------------------------------------------------
 * void*          aecmInst      Pointer to the AECM instance
 *
 * Outputs                      Description
 * -------------------------------------------------------------------
 * int32_t        return        11000-11100: error code
 */
int32_t WebRtcAecm_get_error_code(void *aecmInst);


//--------------------------------------------------------------------
//  Get/Set the ECstartup flag
//--------------------------------------------------------------------

int get_ECstartup(void *aecmInst);

void set_ECstartup(void *aecmInst, int ECstartup);


#ifdef __cplusplus
}
#endif
#endif  // WEBRTC_MODULES_AUDIO_PROCESSING_AECM_INCLUDE_ECHO_CONTROL_MOBILE_H_
