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
1. audio patch, for example, capture I2S0's data and directly send to I2S1 by hardware.
2. before using this example, please check the application note to see how to choose audio architecture and compile.
3. please check the application note to see how to use interfaces.

## Supported IC <a name = "supported-ic"></a>
1. AmebaDplus(only support passthrough architecture)

## Configurations <a name = "configurations"></a>

1. Please see discriptions in component/soc/usrcfg/xx/include/ameba_audio_hw_usrcfg.h.
ameba_audio_hw_usrcfg.h is for audio Hardware configurations.
2. Please set component/soc/usrcfg/xxx/ameba_bootcfg.c's Boot_SocClk_Info_Idx to choose right cpu pll.
take amebadplus for example: 48000 rate, choose PLL_688P128M. 44100 rate choose PLL_677P376M.

### Hardware configurations

1. Setup the hardware pins, like amplifier pins and so on.
2. Please refer to component/audio/audio_hal/xx/README.md.

## How to run <a name = "How to run"></a>
1. Use CMD `./build.py -a audio_manager -p` to compile this example.
2. For playing run command and parameters, please refer to app_example.c.