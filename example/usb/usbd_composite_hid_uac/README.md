# Example Description

In this application, Ameba is designed as a USB Type-C headset: a USB UAC and HID composite device. The USB host recognizes Ameba as an audio speaker (and a microphone with the record option) plus the standard headset buttons on the HID Consumer Control page, Play/Pause, Volume Up/Down, Voice Command and Mute, as the Android USB headset specification defines. They work with the inbox drivers of Windows, macOS, Linux, Android and iOS, no vendor tool is needed.

The interfaces are laid out as on a Type-C headset: the UAC interfaces come first, the HID button interface last.

# HW Configuration

None

# SW configuration

1. Default configuration

	The `prj.conf` of this example sets the options below by default, no menuconfig is needed to run it:
	```
	CONFIG USB --->
	[*] Enable USB
			USB Mode (Device)  --->
		*** USB Device Global Settings ***
	[*] Composite function enable
		*** USB Device Class Selection ***
	[*] HID
			Select HID device type (Headset buttons)  --->
	[*] UAC
			Select UAC Version (UAC 2.0)  --->
	[*]     Enable record (microphone)
	```

2. Build

	```
	./ameba.py build -a usbd_composite_hid_uac -p
	```
	`-p` (pristine) drops the previous configuration and applies the `prj.conf` of this example.

3. (Optional) Change the configuration

	Only needed when the default configuration does not fit:
	1. Build once with `-p` as in step 2, so that the configuration starts from `prj.conf`.
	2. Run `./ameba.py menuconfig` and change the options.
	3. Rebuild without `-p`: `./ameba.py build -a usbd_composite_hid_uac`.

	To keep a change permanently, add its `CONFIG_XXX=y` lines to `prj.conf` instead and build with `-p`.

	The `prj.conf` of this example selects UAC 2.0 (`CONFIG_USBD_UAC2_MENU=y`); the playback is 2 channels, 16-bit and 48 kHz max by default.

	The microphone is on by default (`CONFIG_USBD_UAC_RECORD=y` in `prj.conf`, menuconfig `Enable record (microphone)`): it is reported to the host and `AT+USBDHIDUAC=record` is registered. Remove the line to build playback only. UAC 1.0 and UAC 2.0 both support it, and both run at High Speed on an HS-capable SoC. The microphone streams the single format chosen under `Enable record (microphone)`: `Record channels`, `Record bit width` and `Record sampling rate`, 16 kHz, 2 channels, 16-bit by default. The built-in clip of the example is in that format, so the build stops with an error for any other one.

	Asynchronous playback with a feedback endpoint is off by default. Add `CONFIG_USBD_UAC_PLAY_FEEDBACK=y` to `prj.conf` (menuconfig `UAC ---> Enable playback feedback endpoint (asynchronous)`) to declare the playback endpoint asynchronous with an explicit feedback ISOC IN endpoint (Ref USB 2.0 5.12.4.2, UAC 1.0 and UAC 2.0 alike): once per ms the device reports the rate it consumes, and the host adjusts the playback rate to the device clock instead of its own, so the playback buffer neither overruns nor underruns over a long stream. The example reports the nominal rate corrected by the fill level of the playback ring (kept half full, within 0.1% of the nominal rate), see `composite_uac_cb_get_play_feedback()`. The feedback, like the microphone, is an ISOC IN endpoint, so the SoC limits apply:

	| SoC | IN endpoints | ISOC IN | Record + feedback | Endpoints of this example |
	| --- | --- | --- | --- | --- |
	| RTL8730E, RTL8721Dx (shared DFIFO) | EP1, EP3, EP5 | 1 | No, one of the two | record or feedback EP3 IN (`0x83`) |
	| RTL8721F (dedicated TxFIFOs) | every EP, one TxFIFO each | several | Yes | record EP4 IN (`0x84`), feedback EP3 IN (`0x83`) |

	On RTL8730E / RTL8721Dx enabling both stops the build with `#error "UAC: this SoC (shared DFIFO ...) supports one ISOC IN endpoint only ..."`: remove `CONFIG_USBD_UAC_RECORD=y` from `prj.conf` to build the feedback (`AT+USBDHIDUAC=record` is then not registered). An application that adds more IN endpoints shall count them, the ISOC IN ones in particular, against these limits, refer to the ISOC IN endpoint budget in `usbd_uac.h`. The example also enables the SOF interrupt, which paces the feedback, and on RTL8721F gives the feedback endpoint its own TxFIFO. On a Linux host `cat /proc/asound/card<card>/stream0` shows `Endpoint: <ep> OUT (ASYNC)` and the feedback endpoint (`Sync Endpoint`) once it is built in.

	On the SoCs whose audio driver supports USB audio (RTL8730E, RTL8721Dx), selecting `UAC` also enables the audio framework (`CONFIG APPLICATION ---> Audio Config ---> Enable Audio Framework`), keep its default interface:
	```
	[*] Enable Audio Framework
			Select Audio Interfaces (Mixer)  --->

	Note: For details on audio configuration, please refer to https://aiot.realmcu.com/en/latest/rtos/multimedia/audio/index.html
	```
	On RTL8721F the example is built without the audio framework (the SoC has no `SUPPORT_AUDIO_FOR_USB`), so no OS change is needed: the received audio stream is only drained and counted, see step 4 of Expect result.

4. Download

	Flash the images to the board with any of the tools below:
	* VS Code extension: https://aiot.realmcu.com/en/latest/tools/vscode/index.html
	* Image Tool (Windows): https://aiot.realmcu.com/en/latest/tools/image_tool/index.html
	* Flash Tool (cross-platform): https://aiot.realmcu.com/en/latest/tools/flash/index.html
	* Web flash tool: https://aiot.realmcu.com/en/resource/web_flash_tool.html

# Expect result

1. Reset the board, following log shall be printed on the LOGUART console. The test PASSes only if the lines below show up and no error level log (`-E`) is reported:
	```
	[COMP-I] USBD HID+UAC comp demo start
	[USBD-A] INIT
	[COMP-I] ATTACHED
	```
	Notes:
	* `[USBD-A] INIT` comes from the USB device core, not from the example.
	* `[COMP-I] ATTACHED` is printed once the USB host has enumerated the device, so it only shows up after the cable is connected.
	* On AmebaGreen2 an extra `[USB-I] UPHY para from ...` line is printed by the USB HAL before `[USBD-A] INIT`.

2. Connect the USB port of Ameba board to USB UAC HID host (for example PC) with USB cable.

3. After the USB Audio driver is successfully loaded, USB host end will recognize Ameba as an audio device.

4. Once the host selects an audio format and starts the stream, the playback worker brings up the audio track and following log is printed:
	```
	[COMP-I] Audio track demo begin
	[COMP-I] Audio ch:2,rate:48000,bits=16
	[COMP-I] Track buf size:<value>
	[COMP-I] Track buf size:<value>
	[COMP-I] UAC stop 0
	```
	The reported channel count, sample rate and bit width shall match the format selected on the host side: playback is 44.1 kHz or 48 kHz, 2 channels, 16-bit with the default `UAC` menu. `Track buf size` is printed twice, once when the size is computed and once after the track is initialized; if the audio framework reports no minimum size, the first line reads `Track buf resize to <value>` instead. The board plays 2 channels: a stereo stream is played as is, and with `COMP_UAC_DEMUX_CH_DEBUG` set to 1 in `example_usbd_composite_hid_uac.c` (default) a stream of 4 or more channels is cut down to its front left and right channels; set it to 0 to hand every channel to the audio framework. On the RTL8730E EVB (EV730EL0) the left channel of the 3.5 mm earphone jack is routed through jumper J6: short pins 2-3 (PB3 to AOUT_LPE), otherwise only the right ear plays. The playback data path itself prints no log. Every mute/volume change issued by the host prints `[COMP-I] Mute:<0|1> vol:<value>`.

	On SoCs without `SUPPORT_AUDIO_FOR_USB` (RTL8721F) the audio track is not created: only `[COMP-I] Audio track demo begin` is printed, then a `[COMP-I] Audio track get <len> <total>` line every 200 reads, `<total>` shall keep growing while the host plays.

	Play music on a PC, if you connect a speaker to the board, sound will come out
   ```
	- Open the OS Sound Bar, enable the Realtek HID+UAC Composite Device speaker device if it is disabled. Locate the device in the Device Manager and enable it manually.
	- Open the OS Sound Bar, set Realtek HID+UAC Composite Device Speaker as default device.
	- Open the Sound Control Panel, select the Realtek HID+UAC Composite Device Speaker -- configure the speakers -- choose a specific channel setup -- finalize the configuration, and then click Test.
	- If you connect a speaker to the board, sound will play.
   ```

	On a Linux PC, find the card with `aplay -l` / `arecord -l` (`Realtek HID+UAC Composite Device`), then play a file or a test tone on it:
	```
	aplay -D plughw:<card>,0 test.wav
	speaker-test -D plughw:<card>,0 -c 2 -r 48000 -F S16_LE -t sine
	```
	Change the volume or mute the `Realtek HID+UAC Composite Device` card in `alsamixer -c <card>`, each change prints `[COMP-I] Mute:<0|1> vol:<value>`.

	With the record option built (`CONFIG_USBD_UAC_RECORD=y`, on by default), on any supported SoC, the microphone streams 16 kHz, 2 channels, 16-bit. The following guide demonstrates how to capture and verify the recording data path using Windows 11 and Audacity as the test environment.
	```
	- Configure Recording Software: Launch Audacity. Navigate to Audio Setup > Recording Device and select the target device: Microphone (Realtek HID+UAC Composite Device).
	- Initialize Data Stream: Click the Record button on the Audacity toolbar to prepare the Host to receive audio frames.
	- Execute Test Command: In the device's serial console (UART), execute "AT+USBDHIDUAC=record" command to trigger data transmission.
	- Verify Output: Audacity receives the spoken word "Realtek" repeated once per second. Stop, then play back the track: each repetition shall sound the same, a click, a gap or a pitch shift indicates lost or misplaced packets.
	```
	On a Linux PC, start the capture first, then trigger the replay on the board:
	```
	arecord -D plughw:<card>,0 -c 2 -r 16000 -f S16_LE -d 10 rec.wav     # on the PC
	AT+USBDHIDUAC=record                                                    # on the board
	aplay rec.wav                                                           # on the PC, the looping clip shall be heard
	```
	Triggering the record command prints:
	```
	[COMP-I] UAC record start
	```
	If the host has not configured the device yet, `[COMP-E] UAC start record fail` is printed instead and the session is dropped.

5. Headset button check: type the commands below in the LOGUART console, each clicks one button (press, then release), the host reacts as to the buttons of a Type-C headset:
	```
	AT+USBDHIDUAC=key,volup      # Volume Up: the OS volume bar pops up and the volume goes up
	AT+USBDHIDUAC=key,voldown    # Volume Down
	AT+USBDHIDUAC=key,mute       # Mute toggle
	AT+USBDHIDUAC=key,play       # Play/Pause the media player, on a phone it also answers / hangs up a call
	AT+USBDHIDUAC=key,voice      # Voice Command: launches the voice assistant on Android, ignored by most PCs
	```
	A headset that lacks a button just never reports it, the host needs nothing more.

	On a Linux PC, run `sudo evtest` and select the event device of `Realtek HID+UAC Composite Device`: each command prints a press (value 1) then a release (value 0) of `KEY_PLAYPAUSE`, `KEY_VOLUMEUP`, `KEY_VOLUMEDOWN`, `KEY_VOICECOMMAND` or `KEY_MUTE`.

6. Hotplug check: the example tears down and re-inits the whole composite stack on every detach, so unplug and re-plug the cable, following log shall be printed for each cycle:
	```
	[COMP-I] DETACHED
	[USBD-A] DEINIT
	[COMP-I] Free heap: 0x<value>
	[USBD-A] INIT
	[COMP-I] ATTACHED
	```
	The `Free heap` value shall stay stable across cycles, a value that keeps dropping indicates a memory leak.

# Shell commands

| Command | Description |
| --- | --- |
| `AT+USBDHIDUAC=help` | Print the usage, also when the example is not running. |
| `AT+USBDHIDUAC=key,<play\|volup\|voldown\|voice\|mute>` | Click one headset button: report it pressed, then released. `play`: Play/Pause, `volup`/`voldown`: Volume Up/Down, `voice`: Voice Command, `mute`: Mute. |
| `AT+USBDHIDUAC=record` | Start the microphone (USB IN) replay, which loops a built-in 1 s clip of the spoken word "Realtek" (16 kHz, 2 ch, 16-bit) to the host. Each call queues one more session. Only with the record option built. |

Note: `record` is only registered when `CONFIG_USBD_UAC_RECORD=y`, on in the default `prj.conf`.

Errors:
* `AT+USBDHIDUAC` without a parameter prints the help and ends with `ERROR: -2`.
* Any sub command other than `help` while the example is not running prints `[COMP-E] Not running` and ends with `ERROR: -7`.
* A bad or missing key name prints `[COMP-E] Usage: AT+USBDHIDUAC=key,<play|volup|voldown|voice|mute>`, `record` with an argument prints `[COMP-E] Usage: AT+USBDHIDUAC=record`, both end with `ERROR: -2`.
* An unknown sub command, also `record` on a build without it, prints `[COMP-E] Invalid cmd: <cmd>, try AT+USBDHIDUAC=help` and ends with `ERROR: -2`.
* If a key report can not be sent (e.g. the host has not configured the device, or the stack is being re-initialized after a detach), `[COMP-W] HID key fail <ret>` is printed and the command ends with `ERROR: <ret>`.
* `record` ends with `OK` once the session is queued; a session that can not start prints `[COMP-E] UAC start record fail` later.

# Note

The example needs the HID device type `Headset buttons` (`CONFIG_USBD_HID_HEADSET`), the build stops with an error otherwise. For a vendor defined HID protocol refer to the `usbd_hid` example instead.

Windows caches the descriptors of a device per VID/PID. After a firmware update that changes the UAC or HID descriptors (e.g. channel count, microphone enabled or not), uninstall the device in Device Manager (with its driver) and replug it, otherwise the host may keep using the old layout and the audio endpoints do not work.

# Supported IC

RTL8730E
RTL8721Dx
RTL8721F
