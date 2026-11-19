# Example Description

In this application, Ameba is designed as a UAC (USB Audio Class) device, UAC 1.0 by default (`prj.conf`) or UAC 2.0. USB host (e.g. Windows PC) can recognize Ameba as an audio device with speaker, then communicates with it via USB interface. With the record option (`CONFIG_USBD_UAC_RECORD`, on by default in `prj.conf`) the device also exposes a microphone, which loops a built-in clip of the spoken word "Realtek" to the host on demand.

Both UAC versions run at High Speed on an HS-capable SoC and at Full Speed on an FS-only one. UAC 1.0 keeps one packet per ms at both speeds (bInterval 4 at High Speed, 1 at Full Speed), UAC 2.0 sends one packet per microframe at High Speed.

# HW Configuration

None

# SW configuration

1. Default configuration

	The `prj.conf` of this example sets the options below by default, no menuconfig is needed to run it:
	```
	CONFIG USB --->
	[*] Enable USB
			USB Mode (Device)  --->
	[*] 	UAC
				Select UAC Version (UAC 1.0)  --->
	[*]     	Enable record (microphone)
	```

2. Build

	```
	./ameba.py build -a usbd_uac -p
	```
	`-p` (pristine) drops the previous configuration and applies the `prj.conf` of this example.

3. (Optional) Change the configuration

	Only needed when the default configuration does not fit:
	1. Build once with `-p` as in step 2, so that the configuration starts from `prj.conf`.
	2. Run `./ameba.py menuconfig` and change the options.
	3. Rebuild without `-p`: `./ameba.py build -a usbd_uac`.

	To keep a change permanently, add its `CONFIG_XXX=y` lines to `prj.conf` instead and build with `-p`.

	The default `prj.conf` selects UAC 1.0 with the microphone (`CONFIG_USBD_UAC1_MENU=y`, `CONFIG_USBD_UAC_RECORD=y`). To build UAC 2.0, replace the UAC 1.0 line in `prj.conf` with `CONFIG_USBD_UAC2_MENU=y`; to build playback only, remove `CONFIG_USBD_UAC_RECORD=y`. The playback max channels (2 by default), bit width (16-bit by default) and, on UAC 2.0 only, max sampling rate (48 kHz by default) are chosen under the same `UAC` menu. The microphone streams the single format chosen under `Enable record (microphone)`: `Record channels`, `Record bit width` and `Record sampling rate`, 16 kHz, 2 channels, 16-bit by default. The built-in clip of the example is in that format, so the build stops with an error for any other one.

	On the SoCs whose audio driver supports USB audio (RTL8730E, RTL8721Dx), selecting `UAC` also enables the audio framework (`CONFIG APPLICATION ---> Audio Config ---> Enable Audio Framework`), keep its default interface:
	```
	[*] Enable Audio Framework
			Select Audio Interfaces (Mixer)  --->

	Note: For details on audio configuration, please refer to https://aiot.realmcu.com/en/latest/rtos/multimedia/audio/index.html
	```
	On RTL8721F the example is built without the audio framework (`CONFIG_SUPPORT_AUDIO_FOR_USB` is not set), so no OS change is needed: the received data is only counted, see step 3 of Expect result.

4. Download

	Flash the images to the board with any of the tools below:
	* VS Code extension: https://aiot.realmcu.com/en/latest/tools/vscode/index.html
	* Image Tool (Windows): https://aiot.realmcu.com/en/latest/tools/image_tool/index.html
	* Flash Tool (cross-platform): https://aiot.realmcu.com/en/latest/tools/flash/index.html
	* Web flash tool: https://aiot.realmcu.com/en/resource/web_flash_tool.html

# Expect result

1. Reset the board, following log shall be printed on the LOGUART console. The test PASSes only if the lines below show up and no error level log (`-E`) is reported:
	```
	[UAC-I] USBD UAC demo start
	[USBD-A] INIT
	[UAC-I] ATTACHED
	```
	Notes:
	* `[USBD-A] INIT` comes from the USB device core, not from the example.
	* `[UAC-I] ATTACHED` is printed once the USB host has enumerated the device, so it only shows up after the cable is connected.
	* On AmebaGreen2 (RTL8721F) an extra USB PHY calibration line is printed by the USB HAL right after `[USBD-A] INIT`: `[USB-I] UPHY para from autoload`, `[USB-I] UPHY para from OTP` or `[USB-I] UPHY OTP flag 0x<flag>, use default`.
	* Failure lines: `[UAC-E] Create USBD UAC thread fail`, `[UAC-E] USB init fail`, `[UAC-E] UAC init fail` or `[UAC-E] Create ... fail`, followed by `[UAC-E] USBD UAC demo aborted` and `[UAC-E] Start fail`.

2. Connect the USB port of Ameba board to USB UAC host (for example PC) with USB cable.

3. Once the host selects an audio format and starts the stream, the playback worker brings up the audio track and following log is printed:
	```
	[UAC-I] Audio track demo begin
	[UAC-I] Audio ch:2,rate:48000,bits=16
	[UAC-I] Track buf size:<value>
	[UAC-I] UAC stop 0
	```
	The reported channel count, sample rate and bit width shall match the format selected on the host side (UAC 1.0 declares 44.1 kHz and 48 kHz; UAC 2.0 declares 44.1 kHz up to the configured max rate). With `USBD_UAC_DEMUX_CH_DEBUG` set to 1 in `example_usbd_uac.c` (default), only the first channel of the stream is played on the board, set it to 0 to play every channel. The playback data path itself prints no log. Every time the host switches the format, `[UAC-I] Audio track demo stop` is printed and the four lines above are printed again for the new format. On a SoC without `CONFIG_SUPPORT_AUDIO_FOR_USB` (RTL8721F) the audio track is not created and none of the lines above is printed: a `[UAC-I] Audio track get <len> <total>` line is printed every 200 reads instead, `<total>` shall keep growing while the host plays.

4. After the USB Audio driver is successfully loaded, USB host end will recognize Ameba as an audio device. The device is named `Realtek UAC1.0 Device` on a UAC 1.0 build (default) and `Realtek UAC2.0 Device` on a UAC 2.0 build.
	- Windows:
		- Open `Settings -> System -> Sound` (or the Sound Control Panel), enable the `Realtek UAC1.0 Device` speaker if it is disabled, and set it as the default output device. If it does not show up, locate the device in the Device Manager and enable it manually.
		- In the Sound Control Panel, select the speaker -- configure the speakers -- choose a specific channel setup -- finalize the configuration, and then click Test.
		- With the record option, the microphone shows up under `Input`: run `AT+USBDUAC=record` and check the input level meter, or record it with Audacity, refer to step 5.
	- Linux:
		```
		aplay -l                                                  # find the card number of the device
		aplay -D plughw:<card>,0 -f S16_LE -r 48000 -c 2 test.wav
		```
		With the record option, start the device clip with `AT+USBDUAC=record`, then record and play back the result, the word "Realtek" is heard once per second (or open `rec.wav` in Audacity):
		```
		arecord -l
		arecord -D hw:<card>,0 -f S16_LE -r 16000 -c 2 -d 5 rec.wav
		```
	- If you connect a speaker to the board, sound will play.

5. Now user can play audio via the audio device.

	With the record option built, the following guide demonstrates how to capture and verify the recording data path using Windows 11 and Audacity as the test environment.
	```
	- Configure Recording Software: Launch Audacity. Navigate to Audio Setup > Recording Device and select the target device: Microphone (Realtek UAC1.0 Device, or Realtek UAC2.0 Device on a UAC 2.0 build).
	- Initialize Data Stream: Click the Record button on the Audacity toolbar to prepare the Host to receive audio frames.
	- Execute Test Command: In the device's serial console (UART), execute "AT+USBDUAC=record" command to trigger data transmission.
	- Verify Output: Audacity receives the spoken word "Realtek" repeated once per second. Stop, then play back the track: each repetition shall sound the same, a click, a gap or a pitch shift indicates lost or misplaced packets.
	```
	Triggering the record command prints:
	```
	[UAC-I] UAC record start
	```

6. Hotplug check: the example tears down and re-inits the USB stack on every detach, so unplug and re-plug the cable, following log shall be printed for each cycle:
	```
	[UAC-I] DETACHED
	[USBD-A] DEINIT
	[UAC-I] Free heap: 0x<value>
	[USBD-A] INIT
	[UAC-I] ATTACHED
	```
	The `Free heap` value shall stay stable across cycles, a value that keeps dropping indicates a memory leak.

# Shell commands

| Command | Description |
| --- | --- |
| `AT+USBDUAC=record` | Start the microphone (USB IN) replay, which loops a built-in 1 s clip of the spoken word "Realtek" (16 kHz, 16-bit, 2 channels) to the host. Each call queues one more session. Prints `[UAC-I] UAC record start` once the host has enumerated the device, the clip is then streamed until the next detach or stop; a call before the enumeration prints `[UAC-E] UAC start record fail` instead. |
| `AT+USBDUAC=help` or `AT+USBDUAC=?` | Print the usage. |

Note: the command is only registered when the record option is built (`CONFIG_USBD_UAC_RECORD=y`), with UAC 1.0 or UAC 2.0 and at High or Full Speed alike. The default `prj.conf` enables it. `record` is refused with `[UAC-E] Not running` while the example is not running. The command ends with `OK`, or with `ERROR: <err>` on failure.

# Note

Windows caches the descriptors of a device per VID/PID. After a firmware update that changes the UAC descriptors (e.g. channel count, microphone enabled or not), uninstall the device in Device Manager (with its driver) and replug it, otherwise the host may keep using the old layout and the audio endpoints do not work.

# Supported IC

RTL8730E
RTL8721Dx
RTL8721F
