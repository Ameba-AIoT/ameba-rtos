# Ameba Audio Hardware Configurations

## Table of Contents

- [Ameba Audio Hardware Configurations](#ameba-audio-hardware-configurations)
  - [Table of Contents](#table-of-contents)
  - [About ](#about-)
  - [Configurations ](#configurations-)
    - [internal playback](#internal-playback)
    - [I2S playback](#i2s-playback)
    - [internal record](#internal-record)
    - [I2S record](#i2s-record)

## About <a name = "about"></a>

Ameba audio project can achieve:
1. audio internal codec internal playback.
2. audio external I2S playback.
3. before settings this configurations, please check the application note to see how to choose audio architecture and compile.
4. please check the application note to see how to use audio hal interfaces.

## Configurations <a name = "configurations"></a>

Please see discriptions in component/soc/**/usrcfg/include/ameba_audio_hw_usrcfg.h.

### internal playback

For audio internal playback, the default setting is using internal codec.
Please make sure your amplifier configurations are correctly set.

```
    //Audio amplifier pin, if your board has no audio amplifier pin , please set it as -1.
    #define AUDIO_HW_AMPLIFIER_PIN          _PB_31

    //After enable amplifer, it need some time to be steady.
    #define AUDIO_HW_AMPLIFIER_ENABLE_TIME  90

    //After disable amplifer, it need some time to be steady.
    #define AUDIO_HW_AMPLIFIER_DISABLE_TIME 1

    /*
     *When no sound playing, should amplifier mute? Set 1 if user wants amplifier mute, set
     *0 if user wants amplifier stay unmute.
     *For our demo board, our hardware design makes sure aec noise acceptable, if
     *amplifier stays unmute, so it can be set 0.
     */
    #define AUDIO_HW_AMPLIFIER_MUTE_ENABLE  0
```

### I2S playback

If you want to play with I2S, please change the following codes:

```
    component/audio/audio_hal/ameba**/primary_audio_hw_stream_out.c
    out->out_pcm = ameba_audio_stream_tx_init(AMEBA_AUDIO_DEVICE_I2S, out->config);

    component/soc/**/usrcfg/include/ameba_audio_hw_usrcfg.h

    //define your I2S out sport index and I2S out devices of your board.
    #define AUDIO_I2S_OUT_SPORT_INDEX        1
    #define AUDIO_I2S_OUT_MCLK_PIN           _PA_11
    #define AUDIO_I2S_OUT_BCLK_PIN           _PA_12
    #define AUDIO_I2S_OUT_LRCLK_PIN          _PA_21
    #define AUDIO_I2S_OUT_DATA0_PIN          _PA_22
    #define AUDIO_I2S_OUT_DATA1_PIN          _PA_23
    #define AUDIO_I2S_OUT_DATA2_PIN          _PA_24
    #define AUDIO_I2S_OUT_DATA3_PIN          _PA_25

    //if using TDM, AUDIO_I2S_OUT_MULTIIO_EN set as 0, otherwise as 1.
    #define AUDIO_I2S_OUT_MULTIIO_EN         0


```

For I2S, if you need to add logic in our codes to avoid amplifier noise.
Here is the time sequence of our playback:

```
    1. when create hal's stream out, ameba_audio_stream_render.c's API ameba_audio_stream_tx_sport_init will be called, in which bclk clock will be generated.
    2. if hal is in standby mode, and user write first data, hal will exit standby and call ameba_audio_stream_tx_set_amp_state(true), amplifier will be enabled.
    3. if hal enters standby mode, ameba_audio_stream_tx_set_amp_state(false) will be called to disable amplifer.
    4. when destroy hal's stream out, first hal enters standby mode, and disable amplifier, then ameba_audio_stream_render.c's API ameba_audio_stream_tx_close will be called, and bclk clock will stop output.
```

Here are the suggestions for you to add related codes:

```
    1. set ameba_audio_hw_usrcfg.h's AUDIO_HW_AMPLIFIER_MUTE_ENABLE as 1, set AUDIO_HW_AMPLIFIER_TYPE as 0, if you have your own amplifier.
    2. add amplifer's implentations in component/audio/audio_driver/amp_manager/dummy.c
    3. if you want to do some initialization work of amplifier, the codes can be added in ameba_audio_stream_render.c's API ameba_audio_stream_tx_init.
       after line:ameba_audio_stream_tx_sport_init(&rstream, config, device);
    4. in ameba_audio_stream_control.c's ameba_audio_ctl_set_amp_state, add amplifer enable, and disable implementations.
    5. if you want to deinit amplifier, the codes can be added in ameba_audio_stream_render.c's API ameba_audio_stream_tx_close, before line:AUDIO_SP_Deinit.
```

### internal record

For audio internal record, the default setting is using amic.
If you want to use dmic, please set following configurations.

```
    //for example, define audio internal record dmic devices as follows:
    #define AUDIO_HW_DMIC_CLK_PIN           _PA_23
    #define AUDIO_HW_DMIC_DATA0_PIN         _PA_24
    #define AUDIO_HW_DMIC_DATA1_PIN         _PA_28

    //After dmic clock enable, it need some time to be steady. Please set it's millisecond value here.
    #define AUDIO_HW_DMIC_STEADY_TIME       100
```

### I2S record
If you want to record with I2S, please change the following codes:

```
    component/soc/**/usrcfg/include/ameba_audio_hw_usrcfg.h

    //define your I2S in sport index and I2S in devices of your board.
    #define AUDIO_I2S_IN_SPORT_INDEX        1
    #define AUDIO_I2S_IN_MCLK_PIN           _PA_11
    #define AUDIO_I2S_IN_BCLK_PIN           _PA_26
    #define AUDIO_I2S_IN_LRCLK_PIN          _PA_21
    #define AUDIO_I2S_IN_DATA0_PIN          _PA_22
    #define AUDIO_I2S_IN_DATA1_PIN          _PA_23
    #define AUDIO_I2S_IN_DATA2_PIN          _PA_24
    #define AUDIO_I2S_IN_DATA3_PIN          _PA_25

    //if using TDM, AUDIO_I2S_IN_MULTIIO_EN set as 0, otherwise as 1.
    #define AUDIO_I2S_IN_MULTIIO_EN         0

```