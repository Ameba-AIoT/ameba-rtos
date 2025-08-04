# AmebaSmart Media Playback Via USB Audio Class

## Table of Contents

- [AmebaSmart Media Playback Via USB Audio Class](#amebasmart-media-playback-via-usb-audio-class)
	- [Table of Contents](#table-of-contents)
	- [About](#about)
	- [Supported IC](#supported-ic)
	- [Configurations](#configurations)
		- [Hardware Configurations](#hardware-configurations)
	- [How to build](#how-to-build)
	- [How to run](#how-to-run)
		- [Note](#note)

## About

Amebasmart media project can achieve:
1. audio playback via usb audio class, for example usb earphones.
2. please check the application note to see how to use interfaces.
3. example_player_uac.c gives an example of the detailed implementation of USB host uac and RTPlayer.

## Supported IC
1. AmebaSmart CA32

## Configurations

### Hardware Configurations
Required Components: another Usb audio class, for example usb earphones
Some Ameba boards need to be reworked to support USB host, refer to AN for details.
Taking RTL8730EA/QFN100 boards for example, the register R20 shall be parted on with 0Ohm register or just shorted to provide power for USB device via VBUS.

## How to build
1. **GCC:** cd amebasmart_gcc_project and run `./menuconfig.py` to enable configs.
    ```
    CONFIG Link Option  --->
		IMG2(Application) running on PSRAM or FLASH? (FLASH)  --->
			(X) FLASH

    CONFIG USB --->
        [*] Enable USB
                        USB Mode (Host)  --->
        [*]     UAC

	CONFIG APPLICATION  --->
		Audio Config  --->
			-*- Enable Audio Framework
				Select Audio Interfaces (Mixer)  --->
					(X) Mixer
			[*] Enable Media Framework
				Demux  --->selected supported demux
				Codec  --->selected supported codec
				Sink   --->[*] BT & UAC
			Third Party Lib  --->
				[*] Speex
    ```

2. **GCC:** cd amebasmart_gcc_project and run `./build.py` to compile.

## How to run

1. `Download` images to two devices by Ameba Image Tool.
	```

2. For device:
	```
	AT+WLCONN=ssid,OverWall,pw,12345678 //connect to a wifi AP, ssid:OverWall, pw:12345678

	player_usb -F http://m4a-player.test.upcdn.net/5.m4a
	```

### Note
The player will only start successfully when the usb earphones via VBUS connection is successful.