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
1. audio eq uart setting.
2. only support mixer architecture now:
   ./menuconfig.py  ---->  CONFIG APPLICATION ----> Audio Config ----> Select Audio Interfaces (Mixer)
3. please check the application note to see how to use interfaces.

## Supported IC <a name = "supported-ic"></a>
1. AmebaSmart
2. AmebaLite

## Configurations <a name = "configurations"></a>

Please see discriptions in component/soc/usrcfg/xx/include/ameba_audio_hw_usrcfg.h, ameba_audio_hw_usrcfg.h is for audio hardware configurations.
Please see discriptions in component/audio/configs/include/ameba_audio_mixer_usrcfg.h, and setup component/audio/configs/ameba_audio_mixer_usrcfg.cpp.

### Hardware configurations

1. Setup the hardware pins, like amplifier pins and so on.
2. Define whether using pll clock or xtal clock for playback.
3. Please refer to component/audio/audio_hal/xx/README.md.

## How to run <a name = "How to run"></a>

Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

### Note

1. #define USING_CMD 1 : using audio_uart cmds to start communicating with PC.
2. #define USING_CMD 0 : using app_example interface to start communicating with PC directly after system boot without any cmd.
                         If user compile image without Enabling WIFI Config, please set USING_CMD 0 here.
4. Please change uart pinmux and uart index according to your board, for example:
   ```c
   #define AUDIO_UART_INDEX                1
   #define AUDIO_UART_TX_PIN               PB_11
   #define AUDIO_UART_RX_PIN               PB_10
   ```
3. Use CMD `./build.py` to compile this example.
   If you want to hear the effect, you can also ./build.py -a audio_track.
   enter cmd: audio_uart (with #define USING_CMD 1) and wait 3 seconds for system to be ready.
   enter cmd: audio_track -r 48000 -c 2 -f 16 (not necessary).

4. Run AudioConfigTool

