#include "tinyengine_audio.h"
#include "tinyengine.h"
#include "hardware/pwm.h"

tinyengine_status_t te_audio_init(te_audio_handle_t* audio) {

    gpio_set_function(audio->audio_out_1, GPIO_FUNC_PWM);
    gpio_set_slew_rate(audio->audio_out_1, GPIO_SLEW_RATE_FAST);
    audio->slice_num = pwm_gpio_to_slice_num(audio->audio_out_1);
    pwm_set_clkdiv(audio->slice_num, audio->audio_out_1);
    pwm_set_wrap(audio->slice_num, audio->period);
    pwm_set_chan_level(audio->slice_num, PWM_CHAN_A, audio->period * 0.9);
    pwm_set_chan_level(audio->slice_num, PWM_CHAN_B, audio->period / 3);
    pwm_set_enabled(audio->slice_num, true);

    return TINYENGINE_OK;
}
