# AmebaSmart Media Playback Via Bluetooth

## Table of Contents

- [AmebaSmart Player A2dp](#amebasmart-media-playback-via-bluetooth)
	- [Table of Contents](#table-of-contents)
	- [About](#about)
	- [Supported IC](#supported-ic)
	- [Configurations](#configurations)
		- [Hardware configurations](#hardware-configurations)
		- [Software configurations](#software-configurations)
	- [How to build](#how-to-build)
	- [How to run](#how-to-run)

## About

Amebasmart media project can achieve:
1. audio playback via a2dp.
2. please check the application note to see how to use interfaces.
3. a2dp.c gives an example of the detailed implementation of bluetooth a2dp and RTPlayer.

## Supported IC
1. AmebaSmart CA32

## Configurations

### Hardware Configurations
Required Components: another AmebaSmart device as bluetooth sink device

### Software Configurations
1. **copy** `a2dp.c, CMakeLists.txt` to `component/bluetooth/example/bt_a2dp/`.

## How to build
1. **GCC:** cd amebasmart_gcc_project and run `./menuconfig.py` to enable configs.
    ```
    CONFIG Link Option  --->
		IMG2(Application) running on PSRAM or FLASH? (FLASH)  --->
			(X) FLASH

	CONFIG APPLICATION  --->
		Audio Config  --->
			-*- Enable Audio Framework
				Select Audio Interfaces (Mixer)  --->
					(X) Mixer
			[*] Enable Media Framework
				Demux  --->selected supported demux
				Codec  --->selected supported codec
				Audio Output  --->[*] Bluetooth A2DP Output

	CONFIG BT  --->
		[*] Enable BT
		[*]     BT Example Demo
		[*]         BT A2DP
    ```

2. **GCC:** cd amebasmart_gcc_project and run `./build.py` to compile.

## How to run
1. Use one amebasmart device as bluetooth a2dp source, another amebasmart device as bluetooth a2dp sink.

2. `Download` images to two devices by Ameba Image Tool.

3. For a2dp sink device:
	```
	AT+BTDEMO=a2dp,snk,1
	```

4. For a2dp source device:
	```
	AT+WLCONN=ssid,OverWall,pw,12345678 //connect to a wifi AP, ssid:OverWall, pw:12345678

	AT+BTDEMO=a2dp,src,1
	AT+BTA2DP=conn,sink device mac //ex: AT+BTA2DP=conn,00e04c800499
	AT+BTA2DP=start,sink device mac //ex: AT+BTA2DP=start,00e04c800499

	player_a2dp -nt 0 -F http://m4a-player.test.upcdn.net/3_441k_2_3min.mp3
	```

### Note
1. The player will only start successfully when the bluetooth connection is successful.
2. Once successfully played, if bluetooth transmission fails, the playback will automatically exit.
3. player_a2dp [OPTION...]
	```
	[-nt longrun flag]     0: play once, 1: play longrun
	[-F audio_path]        An audio file path to play
	[-p pause_at]          Pause audio $(pause_at) ms after playing start or resume
	[-r resume_at]         Resume audio $(resume_at) ms after pause
	[-ra rewind_at]        Start rewind when current position is $(rewind_at) ms
	[-rt rewind_to]        Rewind to $(rewind_to) ms, this variable needs to be used with [-ra rewind_at]
	```

	Examples:
	```
	1. loop playing music:
		player_a2dp -nt 1 -F file1.wav
	2. pause & resume
		player_a2dp -nt 0 -F file1.wav -p 5000 -r 2000
	3. playing->rewind
		player_a2dp -nt 0 -F file1.wav -ra 10000 -st 58000 -ra 90000 -rt 10000
	```