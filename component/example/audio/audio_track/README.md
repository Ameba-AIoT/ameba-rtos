# Ameba Audio

## Table of Contents

- [Ameba Audio](#ameba-audio)
	- [Table of Contents](#table-of-contents)
	- [About ](#about-)
	- [Supported IC ](#supported-ic-)
	- [Configurations ](#configurations-)
		- [Hardware configurations](#hardware-configurations)
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

Please see discriptions in component/soc/xx/usrcfg/include/ameba_audio_hw_usrcfg.h.
ameba_audio_hw_usrcfg.h is for audio Hardware configurations.

### Hardware configurations

1. Setup the hardware pins, like amplifier pins and so on.
2. Define whether using pll clock or xtal clock for playback.
3. Please refer to component/audio/audio_hal/xx/README.md.

## How to run <a name = "How to run"></a>
1. Use CMD `make EXAMPLE=audio_track` to compile this example.
2. For playing run command and parameters, please refer to app_example.c.