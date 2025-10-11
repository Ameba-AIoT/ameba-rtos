# Ameba Audio

## Table of Contents

- [Ameba Audio](#ameba-audio)
	- [Table of Contents](#table-of-contents)
	- [About](#about)
	- [Supported IC](#supported-ic)
	- [Configurations](#configurations)
		- [Hardware configurations](#hardware-configurations)
	- [Software Patches](#software-patches)
		- [A2dp Hal](#hal-patch)
		- [A2dp Demo](#a2dp-patch)
	- [How to run](#how-to-run)

## About

Ameba audio project can achieve:
1. if a2dp connected, using chip as a2dp source, and play audio through a2dp.
2. if a2dp disconnected, play through speaker.

## Supported IC
1. AmebaSmart

## Configurations

Please see discriptions in component/soc/usrcfg/xx/include/ameba_audio_hw_usrcfg.h, ameba_audio_hw_usrcfg.h is for audio hardware configurations.
Please see discriptions in component/audio/configs/include/ameba_audio_mixer_usrcfg.h, and setup component/audio/configs/ameba_audio_mixer_usrcfg.cpp.
Please see discriptions in component/audio/configs/include/ameba_audio_policy_configs.h, and setup component/audio/configs/audio_policy/ameba**/ameba_audio_policy_configs.c

Enter directory:amebasmart_gcc_project
./menuconfig.py--->CONFIG APPLICATION--->AUDIO CONFIG--->Select Audio Interface(Mixer)
./menuconfig.py--->CONFIG APPLICATION--->AUDIO CONFIG--->Audio Devices--->Bluetooth A2DP Device
./menuconfig.py--->CONFIG BT--->BT Example Demo--->BT A2DP

### Hardware configurations

1. Setup the hardware pins, like amplifier pins and so on.
2. Define whether using pll clock or xtal clock for playback.
3. Please refer to component/audio/audio_hal/xx/README.md.

## Software Patches
### A2dp Hal

replace component/audio/audio_hal/a2dp/a2dp_audio_hw_stream_out.c with a2dp_audio_hw_stream_out.c in this directory.

### A2dp Demo

patch a2dp.patch to component/bluetooth directory.

## How to run

Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

### Note

1. Use CMD `./build.py -a audio_track` to compile track example.

2. One smart board as source, one smart board as slave, running same image.

3. After system boot, sink cmd:
   'AT+BTDEMO=a2dp,snk,1'

4. After system boot, source cmd:
   'AT+BTDEMO=a2dp,src,1'

5. After above steps, we can get bluetooth mac address through logs.

6. Source board connect to sink's bluetooth mac, suppose sink's mac is 00e04c800499
   'AT+BTA2DP=conn,00e04c800499'
   'AT+BTA2DP=start,00e04c800499'

7. Source board run:
   'audio_track -r 16000 -c 2 -f 16'

8. After all above steps, we will here sine wave through sink board.

9. If user wants to disconnect a2dp, use following cmd, and sine will output from other connected device(speaker or uac...).
    AT+BRGAP=disc,00e04c800499

10. If user wants to test a2dp together with uac, user can run uac test together with this test.
    Then user will here a2dp's sine wave and uac's music mixed together and playing with a2dp device and uac device.

11. Notice: running uac and a2dp tests together takes 2.9M heap, please make sure memory is enough.
    adjust amebasmart_gcc_project/ameba_layout.ld if necessary.

12. ameba_audio_mixer_usrcfg.cpp config setting is very very important. If you have distortion, please change kMultipleCopiedBuffer value.




