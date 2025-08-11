#ifndef __TINYENGINE_RENDERER_DVI_H__
#define __TINYENGINE_RENDERER_DVI_H__
#include "tinyengine.h"

typedef struct {
    uint audio_out_1, audio_out_2;
    uint period;
    uint slice_num;
} te_audio_handle_t;

tinyengine_status_t te_audio_init(te_audio_handle_t* audio);

#endif