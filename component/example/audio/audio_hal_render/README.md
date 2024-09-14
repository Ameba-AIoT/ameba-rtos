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
3. AmebaDplus

## Configurations <a name = "configurations"></a>

Please see discriptions in component/soc/xx/usrcfg/include/ameba_audio_hw_usrcfg.h, ameba_audio_hw_usrcfg.h is for audio hardware configurations.

### Hardware configurations

1. Setup the hardware devices, like amplifier devices and so on.
2. Define whether using pll clock or xtal clock for playback.
3. Please refer to component/audio/audio_hal/xx/README.md.

### Software configurations
Select the core used for application core(KM4 or CA32):
Under "Audio Config"
[*] "Enable Audio Framework"
(X) "PassThrough"
[*]"Enable Media Framework"
"Demux" --->
[*] "Demux FLAC"
[*] "Demux OGG"

## How to run <a name = "How to run"></a>
1. Use CMD `make all EXAMPLE=audio_hal_render` to compile this example.
2. For playing run command and parameters, please refer to app_example.c.
3. To run the example, the default command in serial log UART is: AudioHalRender
