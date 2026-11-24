# Example Description

In this application, Ameba is designed as a USB UVC and UAC composite host. It can connect to composite devices such as a USB camera with a built-in microphone. Ameba receives H264 video frames from the UVC class and captures PCM audio from the UAC microphone.

The composite behavior is achieved at the application level by registering the UVC and UAC standard class drivers to the host core simultaneously, without a dedicated composite class layer.

# HW Configuration

Some Ameba boards require hardware rework to support USB Host mode.
Populate the designated resistor footprint with a **0Ohm resistor** (or apply a solder bridge)
to enable VBUS power supply to the USB device.
Refer to the [EVB User Guide](https://aiot.realmcu.com/filelist?document_type=9) for details.

## Board Rework Reference

### RTL8730E

| Board Variant      | Resistor | Rework Action                                    |
|--------------------|----------|--------------------------------------------------|
| RTL8730EA / QFN100 | R20      | Populate with 0Ohm resistor or solder bridge     |
| RTL8730EL / QFN144 | R166     | Populate with 0Ohm resistor, or use an OTG cable |

### RTL8721F

| Board Variant    | Resistor | Rework Action                                           |
|------------------|----------|---------------------------------------------------------|
| RTL8721FC0-R00   | R52      | Populate with 0Ohm resistor                             |
| RTL8721FA0-3V0   | R52      | Populate with 0Ohm resistor                             |
| RTL8721FL0-2V0   | R2       | Populate with 0Ohm resistor *(pre-reworked by default)* |

# SW configuration

1. Default configuration

	The `prj.conf` of this example sets the options below by default, no menuconfig is needed to run it:
	```
	CONFIG USB --->
	[*] Enable USB
	        USB Mode (Host)  --->
	[*] UVC
	[*] UAC
	```

2. Build

	```
	./ameba.py build -a usbh_composite_uvc_uac -p
	```
	`-p` (pristine) drops the previous configuration and applies the `prj.conf` of this example.

3. (Optional) Change the configuration

	Only needed when the default configuration does not fit:
	1. Build once with `-p` as in step 2, so that the configuration starts from `prj.conf`.
	2. Run `./ameba.py menuconfig` and change the options.
	3. Rebuild without `-p`: `./ameba.py build -a usbh_composite_uvc_uac`.

	To keep a change permanently, add its `CONFIG_XXX=y` lines to `prj.conf` instead and build with `-p`.

4. Download

	Flash the images to the board with any of the tools below:
	* VS Code extension: https://aiot.realmcu.com/en/latest/tools/vscode/index.html
	* Image Tool (Windows): https://aiot.realmcu.com/en/latest/tools/image_tool/index.html
	* Flash Tool (cross-platform): https://aiot.realmcu.com/en/latest/tools/flash/index.html
	* Web flash tool: https://aiot.realmcu.com/en/resource/web_flash_tool.html

# Device requirements

* A High-Speed USB camera with a UVC function and, for the audio test, a UAC 1.0 / 2.0 microphone.
* Video: the camera shall advertise H264 at 1920x1080@30fps (`USBH_UVC_STREAM_WIDTH`/`HEIGHT`/`FRAMERATE` in the example); otherwise `UVC setparam fail` is printed and no frame is received. A camera without UAC function is supported, video only.
* Microphone: a mono 48 kHz format is preferred; when the camera has none, `[COMP-W] Rec: 1ch/48000Hz not found, using fmt[0]` is printed and the first listed format is used.
* Speaker (if any): a 2 channel 48 kHz format is preferred, otherwise `[COMP-W] Play: 2ch/48000Hz not found, using fmt[0]` is printed; silence is played.
* Rendering the captured audio through the audio output of the board needs `CONFIG_SUPPORT_AUDIO_FOR_USB` (RTL8730E); on the other SoCs the data is read and dropped.

# Expect result

1. Reset the board, the following log shall be printed on the LOGUART console, make sure there is no USB related error reported (the worker threads print their wait lines on their own, so the order may vary):
	```
	[COMP-I] USBH UVC+UAC comp demo start
	[COMP-I] Ctrl wait setup
	[COMP-I] UVC stream wait
	[COMP-I] Play wait start
	[COMP-I] Rec wait start
	[COMP-I] UVC INIT
	[COMP-I] UAC INIT
	```

2. Connect the USB port of the Ameba board to a USB UVC+UAC composite device (e.g. a USB camera with built-in microphone) with a USB cable.

3. Once the device enumerates successfully, video streaming starts automatically. Frame statistics are reported on the console at 1-second intervals:
	```
	[COMP-I] UVC ATTACH
	[COMP-I] UAC ATTACH
	[COMP-I] UVC SETUP
	[COMP-I] UAC SETUP
	[COMP-I] UVC set param (1920x1080@30fps)
	[COMP-I] UVC setparam status=0
	[COMP-I] UVC streaming started
	[COMP-I] UVC frames: 300 (size=78643, ts=0)
	[COMP-I] UVC frames: 600 (size=78643, ts=0)
	```
	The test FAILs if `UVC setparam req fail`, `UVC setparam fail/timeout`, `UVC setparam fail=<err>` or `UVC start fail` is printed, or the frame count stops increasing. A camera without UAC function prints `[COMP-I] UVC-only device (no UAC attached)` before `UVC set param`.

4. If the device exposes a UAC microphone (ISOC IN), the audio capture pipeline starts automatically. The available audio formats are listed and the preferred format is selected. Audio data statistics are reported every 10 s:
	```
	[COMP-I] Rec fmt list:
	[COMP-I] [0] CH=1 BW=16 FREQ=8000-8000
	[COMP-I] [5] CH=1 BW=16 FREQ=48000-48000
	[COMP-I] UAC rec started
	[COMP-I] Rec data start, frame=<n>
	[COMP-I] Rec loop=940 bytes=960000
	[COMP-I] Rec loop=1880 bytes=1920000
	```
	Each line prints `FREQ=<min>-<max>`: a discrete rate shows the same value twice, a continuous rate range (`bSamFreqType` 0) its bounds; the preferred rate is selected anywhere within a range.
	`[COMP-W] Rec loop=<n> no data`, printed as soon as a second passes without audio, means the stream runs but no audio is received.
	If the device has no speaker output (ISOC OUT not supported), the following message is printed; the play thread then waits for the next device setup, it does not exit:
	```
	[COMP-W] Play not ready or not supported
	```

5. Hotplug check: the example tears down and re-inits the USB stack on every detach, so unplug and re-plug the device, following log shall be printed for each cycle, followed by the log of step 3 (lines of different threads may interleave):
	```
	[COMP-I] UVC DETACH frames=<n>
	[COMP-I] UAC DETACH played=<n> rec=<n>
	[COMP-I] UVC stream stop, total frames=<n>
	[COMP-I] Rec data stop, loops=<n> bytes=<n>
	[COMP-I] 
	========== Hotplug #1 ==========
	[COMP-I] UAC DEINIT
	[COMP-I] UVC DEINIT
	[COMP-I] Free heap: 0x<value>
	[COMP-I] UVC INIT
	[COMP-I] UAC INIT
	[COMP-I] Reinit done
	```
	A `[COMP-W] Play write err <n>` or `[COMP-W] Rec read err <n>` right after the `DETACH` lines is expected: the streams are stopped by the detach. The `Free heap` value shall stay stable across cycles, a value that keeps dropping indicates a memory leak.

6. Make sure there is no error reported during the whole test.

# Shell commands

Type the AT commands on the LOGUART console. Parameters are comma separated; the command name is case-sensitive, the parameters are not. `AT+USBHUVCUAC=help` (or `AT+USBHUVCUAC=?`) prints the usage, every command ends with `OK` or `ERROR: <code>`. Except `help`, the commands are refused with `Not running` while the example is not running.

| Command | Description |
| --- | --- |
| `AT+USBHUVCUAC=init[,<play\|record>]` | Select the stream the following `mute`/`vol` act on, default `play`. Prints `Playback Control` or `Record Control`. |
| `AT+USBHUVCUAC=mute,<0\|1>` | Mute (`1`) or unmute (`0`) the selected stream. Prints `Mute` / `UnMute`, then reads the mute control of every channel back (`Verify mute(<dir>) ...` lines). |
| `AT+USBHUVCUAC=vol,<0..100\|up\|down>` | Set the volume of the selected stream in percent, or step it by 20. Each stream starts at 50. The volume of every channel is read back (`Verify vol(<dir>) ...` lines). |

`mute`/`vol` need an attached and set up device; a camera microphone usually has controls on the `record` stream only. Example:
```
AT+USBHUVCUAC=init,record
AT+USBHUVCUAC=vol,80
AT+USBHUVCUAC=mute,1
AT+USBHUVCUAC=mute,0
```

# Supported IC

RTL8730E
RTL8721F
