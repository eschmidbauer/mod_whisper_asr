/*
 * FreeSWITCH Modular Media Switching Software Library / Soft-Switch Application
 * Copyright (C) 2005-2014, Anthony Minessale II <anthm@freeswitch.org>
 *
 * Version: MPL 1.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * Module Contributor(s):
 *  Konstantin Alexandrin <akscfx@gmail.com>
 *
 *
 */
#ifndef MOD_WHISPER_ASR_H
#define MOD_WHISPER_ASR_H

#include <switch.h>
#include <speex/speex_resampler.h>
#include <whisper.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define MOD_CONFIG_NAME         "whisper_asr.conf"
#define MOD_VERSION             "1.1_19062024"

#define DEF_CHUNK_TIME          15 // sec
#define QUEUE_SIZE              32
#define VAD_STORE_FRAMES        32
#define VAD_RECOVERY_FRAMES     15

typedef struct {
    switch_mutex_t          *mutex;
    const char              *model_file;
    uint32_t                active_threads;
    uint32_t                chunk_time_sec;
    uint32_t                whisper_threads;
    uint32_t                whisper_tokens;
    uint32_t                vad_silence_ms;
    uint32_t                vad_voice_ms;
    uint32_t                vad_threshold;
    uint8_t                 fl_vad_enabled;
    uint8_t                 fl_vad_debug;
    uint8_t                 fl_shutdown;
    //
    uint32_t                whisper_n_threads;
    uint32_t                whisper_max_tokens;
    uint32_t                whisper_flash_attn;
    uint32_t                whisper_use_gpu;
    uint32_t                whisper_gpu_dev;
} globals_t;

typedef struct {
    switch_vad_t            *vad;
    switch_vad_state_t      vad_state;
    switch_buffer_t         *vad_buffer;
    switch_mutex_t          *mutex;
    switch_queue_t          *q_audio;
    switch_queue_t          *q_text;
    SpeexResamplerState     *resampler;
    struct whisper_context  *wctx;
    char                    *lang;
    int32_t                 transcript_results;
    int32_t                 vad_buffer_offs;
    uint32_t                vad_buffer_size;
    uint32_t                vad_stored_frames;
    uint32_t                chunk_buffer_size;
    uint32_t                refs;
    uint32_t                samplerate;
    uint32_t                channels;
    uint32_t                frame_len;
    uint8_t                 fl_pause;
    uint8_t                 fl_vad_enabled;
    uint8_t                 fl_vad_first_cycle;
    uint8_t                 fl_destroyed;
    uint8_t                 fl_abort;
    //
    uint32_t                whisper_max_tokens;
    uint32_t                whisper_translate;
    uint32_t                whisper_single_segment;
} wasr_ctx_t;

typedef struct {
    uint32_t                len;
    switch_byte_t           *data;
} xdata_buffer_t;

/* utils.c */
uint32_t asr_ctx_take(wasr_ctx_t *asr_ctx);
void asr_ctx_release(wasr_ctx_t *asr_ctx);

switch_status_t xdata_buffer_push(switch_queue_t *queue, switch_byte_t *data, uint32_t data_len);
switch_status_t xdata_buffer_alloc(xdata_buffer_t **out, switch_byte_t *data, uint32_t data_len);
void xdata_buffer_free(xdata_buffer_t **buf);
void xdata_buffer_queue_clean(switch_queue_t *queue);

switch_status_t transcribe(wasr_ctx_t *ast_ctx, float *audio, uint32_t samples, switch_buffer_t *text_buffer, globals_t *globals);
void i2f(int16_t *in, float *out, uint32_t samples);

#endif
