# Ameba Audio

## Table of Contents

- [Ameba Audio](#ameba-audio)
	- [Table of Contents](#table-of-contents)
	- [About ](#about-)
	- [Supported IC ](#supported-ic-)
	- [Configurations ](#configurations-)
		- [Hardware configurations](#hardware-configurations)
		- [Software configurations](#software-configurations)
	- [How to run ](#how-to-run-)

## About <a name = "about"></a>

Ameba audio project can achieve:
1. audio playback.
2. before using this example, please check the application note to see how to choose audio architecture and compile.
3. please check the application note to see how to use interfaces.

## Supported IC <a name = "supported-ic"></a>
1. AmebaSmart
2. AmebaLite
3. AmebaDplus(only support passthrough architecture)

## Configurations <a name = "configurations"></a>

Please see discriptions in component/soc/xx/usrcfg/include/ameba_audio_hw_usrcfg.h, and component/usrcfg/common/audio/include/audio/config/ameba_audio_mixer_usrcfg.h.
ameba_audio_hw_usrcfg.h is for audio Hardware configurations, ameba_audio_mixer_usrcfg.h is for audio software mix configurations.

### Hardware configurations

1. Setup the hardware pins, like amplifier pins and so on.
2. Define whether using pll clock or xtal clock for playback.
3. Please refer to component/audio/audio_hal/rtl**/README.md.

### Software configurations

1. Setup audio mixer output samplerate, format, channels.
2. Setup audio mixer architecture's dma period size.
3. Please don't change this configurations unless necessary.

```
//for example, set mixer output rate 44100, channels 2, format 16bit, dma period size 1024 frames, total 4 periods, output frames stage1.
PrimaryAudioConfig kPrimaryAudioConfig = {44100, 2, RTAUDIO_FORMAT_PCM_16_BIT, 1024, 4, RTAUDIO_OUT_MIN_FRAMES_STAGE1}
```

## How to run <a name = "How to run"></a>
1. Use CMD `make EXAMPLE=audio_track` to compile this example.
2. For playing run command and parameters, please refer to app_example.c.
