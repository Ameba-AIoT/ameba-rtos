# Example Description

In this application, Ameba is designed as a USB CDC ACM and UAC composite device, USB host can recognize Ameba as a CDC virtual serial port, an audio speaker and, with the record option (`CONFIG_USBD_UAC_RECORD`, on by default in `prj.conf`), a microphone, which loops a built-in clip of the spoken word "Realtek" to the host on demand.

Both functions run at High Speed on an HS-capable SoC, UAC 1.0 included: it keeps one packet per ms (bInterval 4).

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
	[*] CDC ACM
	[*] UAC
			Select UAC Version (UAC 1.0)  --->
	[*]     Enable record (microphone)
	```
	The `prj.conf` of this example sets these options, UAC 1.0 (`CONFIG_USBD_UAC1_MENU=y`) with the microphone (`CONFIG_USBD_UAC_RECORD=y`), 2 channels and 16-bit by default. Remove `CONFIG_USBD_UAC_RECORD=y` to build playback only. The microphone streams the single format chosen under `Enable record (microphone)`, 16 kHz, 2 channels, 16-bit by default. The built-in clip of the example is in that format, so the build stops with an error for any other one.

2. Build

	```
	./ameba.py build -a usbd_composite_cdc_acm_uac -p
	```
	`-p` (pristine) drops the previous configuration and applies the `prj.conf` of this example.

3. (Optional) Change the configuration

	Only needed when the default configuration does not fit:
	1. Build once with `-p` as in step 2, so that the configuration starts from `prj.conf`.
	2. Run `./ameba.py menuconfig` and change the options.
	3. Rebuild without `-p`: `./ameba.py build -a usbd_composite_cdc_acm_uac`.

	To keep a change permanently, add its `CONFIG_XXX=y` lines to `prj.conf` instead and build with `-p`.

	Asynchronous playback with a feedback endpoint is off by default. Add `CONFIG_USBD_UAC_PLAY_FEEDBACK=y` to `prj.conf` (menuconfig `UAC ---> Enable playback feedback endpoint (asynchronous)`) to declare the playback endpoint asynchronous with an explicit feedback ISOC IN endpoint (Ref USB 2.0 5.12.4.2, UAC 1.0 and UAC 2.0 alike): once per ms the device reports the rate it consumes, and the host adjusts the playback rate to the device clock instead of its own, so the playback buffer neither overruns nor underruns over a long stream. The example reports the nominal rate corrected by the fill level of the playback ring (kept half full, within 0.1% of the nominal rate), see `composite_uac_cb_get_play_feedback()`. The feedback, like the microphone, is an ISOC IN endpoint, so the SoC limits apply:

	| SoC | IN endpoints | ISOC IN | Record + feedback | Endpoints of this example |
	| --- | --- | --- | --- | --- |
	| RTL8730E, RTL8721Dx (shared DFIFO) | EP1, EP3, EP5 | 1 | No, one of the two | record or feedback EP5 IN (`0x85`) |
	| RTL8721F (dedicated TxFIFOs) | every EP, one TxFIFO each | several | Yes | record EP4 IN (`0x84`), feedback EP6 IN (`0x86`) |

	On RTL8730E / RTL8721Dx enabling both stops the build with `#error "UAC: this SoC (shared DFIFO ...) supports one ISOC IN endpoint only ..."`: remove `CONFIG_USBD_UAC_RECORD=y` from `prj.conf` to build the feedback (`AT+USBDACMUAC=record` is then not registered). An application that adds more IN endpoints shall count them, the ISOC IN ones in particular, against these limits, refer to the ISOC IN endpoint budget in `usbd_uac.h`. The example also enables the SOF interrupt, which paces the feedback, and on RTL8721F gives the feedback endpoint its own TxFIFO. On a Linux host `cat /proc/asound/card<card>/stream0` shows `Endpoint: <ep> OUT (ASYNC)` and the feedback endpoint (`Sync Endpoint`) once it is built in.

	On the SoCs whose audio driver supports USB audio (RTL8730E, RTL8721Dx), selecting `UAC` also enables the audio framework (`CONFIG APPLICATION ---> Audio Config ---> Enable Audio Framework`), keep its default interface:
	```
	[*] Enable Audio Framework
			Select Audio Interfaces (Mixer)  --->

	Note: For details on audio configuration, please refer to https://aiot.realmcu.com/en/latest/rtos/multimedia/audio/index.html
	```
	On RTL8721F the example is built without the audio framework (the SoC has no `SUPPORT_AUDIO_FOR_USB`), so no OS change is needed: the received audio stream is only drained and counted, see step 6 of Expect result.

4. Download

	Flash the images to the board with any of the tools below:
	* VS Code extension: https://aiot.realmcu.com/en/latest/tools/vscode/index.html
	* Image Tool (Windows): https://aiot.realmcu.com/en/latest/tools/image_tool/index.html
	* Flash Tool (cross-platform): https://aiot.realmcu.com/en/latest/tools/flash/index.html
	* Web flash tool: https://aiot.realmcu.com/en/resource/web_flash_tool.html

# Expect result

1. Reset the board, following log shall be printed on the LOGUART console. The test PASSes only if the lines below show up and no error level log (`-E`) is reported:
	```
	[COMP-I] USBD ACM+UAC comp demo start
	[USBD-A] INIT
	[COMP-I] ATTACHED
	```
	Notes:
	* `[USBD-A] INIT` comes from the USB device core, not from the example.
	* `[COMP-I] ATTACHED` is printed once the USB host has enumerated the device, so it only shows up after the cable is connected.
	* On AmebaGreen2 an extra `[USB-I] UPHY para from ...` line is printed by the USB HAL before `[USBD-A] INIT`.

2. Connect the USB port of Ameba board to USB UAC host (for example PC) with USB cable.

3. Launch any serial port tool (e.g. Realtek Trace Tool), open the virtual serial port against the USB port of Ameba board, send messages to the Ameba board and the board will echo back the received messages. The echo data path prints no log.
	On Linux the port is `/dev/ttyACMx` (stop ModemManager first, e.g. `sudo systemctl stop ModemManager`, then `picocom -b 115200 /dev/ttyACM0`); on Windows 10/11 it is `USB Serial Device (COMx)`.

4. After the USB Audio driver is successfully loaded, USB host end will recognize Ameba as an audio device.

5. Play music on a PC, if you connect a speaker to the board, sound will come out.
   ```
	- Open the OS Sound Bar, enable the Realtek ACM+UAC Composite Device speaker device if it is disabled. Locate the device in the Device Manager and enable it manually.
	- Open the OS Sound Bar, set Realtek ACM+UAC Composite Device Speaker as default device.
	- Open the Sound Control Panel, select the Realtek ACM+UAC Composite Device Speaker device -- configure the speakers -- choose a specific channel setup -- finalize the configuration, and then click Test.
	- If you connect a speaker to the board, sound will play.
   ```
   On a Linux PC, find the card with `aplay -l` (`Realtek ACM+UAC Composite Device`), then play a file or a test tone on it:
   ```
	aplay -D plughw:<card>,0 test.wav
	speaker-test -D plughw:<card>,0 -c 2 -r 48000 -F S16_LE -t sine
   ```
   The device declares UAC 1.0 playback at 44.1 kHz and 48 kHz, 2 channels, 16-bit (the defaults of the `UAC` menu).

6. Once the host selects an audio format and starts the stream, the playback worker brings up the audio track and following log is printed:
	```
	[COMP-I] Audio track demo begin
	[COMP-I] Audio ch:2,rate:48000,bits=16
	[COMP-I] Track buf size:<value>
	[COMP-I] Track buf size:<value>
	[COMP-I] UAC stop 0
	```
	The reported channel count, sample rate and bit width shall match the format selected on the host side. `Track buf size` is printed twice, once when the size is computed and once after the track is initialized; if the audio framework reports no minimum size, the first line reads `Track buf resize to <value>` instead. The board plays 2 channels: a stereo stream is played as is, and with `COMP_UAC_DEMUX_CH_DEBUG` set to 1 in `example_usbd_composite_cdc_acm_uac.c` (default) a stream of 4 or more channels is cut down to its front left and right channels; set it to 0 to hand every channel to the audio framework. On the RTL8730E EVB (EV730EL0) the left channel of the 3.5 mm earphone jack is routed through jumper J6: short pins 2-3 (PB3 to AOUT_LPE), otherwise only the right ear plays. The playback data path itself prints no log. The lines above are printed again every time the host switches the format.

	On SoCs without `SUPPORT_AUDIO_FOR_USB` (RTL8721F) the audio track is not created: only `[COMP-I] Audio track demo begin` is printed, then a `[COMP-I] Audio track get <len> <total>` line every 200 reads, `<total>` shall keep growing while the host plays.

7. With the record option built, the microphone streams 16 kHz, 2 channels, 16-bit. The following guide demonstrates how to capture and verify the recording data path using Windows 11 and Audacity as the test environment.
	```
	- Configure Recording Software: Launch Audacity. Navigate to Audio Setup > Recording Device and select the target device: Microphone (Realtek ACM+UAC Composite Device).
	- Initialize Data Stream: Click the Record button on the Audacity toolbar to prepare the Host to receive audio frames.
	- Execute Test Command: In the device's serial console (UART), execute "AT+USBDACMUAC=record" command to trigger data transmission.
	- Verify Output: Audacity receives the spoken word "Realtek" repeated once per second. Stop, then play back the track: each repetition shall sound the same, a click, a gap or a pitch shift indicates lost or misplaced packets.
	```
	On a Linux PC, start the capture first, then trigger the replay on the board:
	```
	arecord -D plughw:<card>,0 -c 2 -r 16000 -f S16_LE -d 10 rec.wav     # on the PC
	AT+USBDACMUAC=record                                                    # on the board
	aplay rec.wav                                                           # on the PC, the looping clip shall be heard
	```
	Triggering the record command prints:
	```
	[COMP-I] UAC record start
	```
	If the host has not configured the device yet, `[COMP-E] UAC start record fail` is printed instead and the session is dropped.

8. Hotplug check: the example tears down and re-inits the whole composite stack on every detach, so unplug and re-plug the cable, following log shall be printed for each cycle:
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
| `AT+USBDACMUAC=help` | Print the usage, also when the example is not running. |
| `AT+USBDACMUAC=record` | Start the microphone (USB IN) replay, which loops a built-in 1 s clip of the spoken word "Realtek" (16 kHz, 2 ch, 16-bit) to the host. Each call queues one more session. |

Note: the command is only registered when `CONFIG_USBD_UAC_RECORD=y`, on in the default `prj.conf`.

Errors:
* `AT+USBDACMUAC` without a parameter prints the help and ends with `ERROR: -2`.
* `record` while the example is not running prints `[COMP-E] Not running` and ends with `ERROR: -7`.
* `record` with an argument prints `[COMP-E] Usage: AT+USBDACMUAC=record` and ends with `ERROR: -2`; an unknown sub command prints `[COMP-E] Invalid cmd: <cmd>, try AT+USBDACMUAC=help` and ends with `ERROR: -2`.
* `record` ends with `OK` once the session is queued; a session that can not start prints `[COMP-E] UAC start record fail` later.

# Note

Windows caches the descriptors of a device per VID/PID. After a firmware update that changes the UAC descriptors (e.g. channel count, microphone enabled or not), uninstall the device in Device Manager (with its driver) and replug it, otherwise the host may keep using the old layout and the audio endpoints do not work.

Windows 10/11 needs no driver. Specially for Win7/XP host, please manually install the CDC ACM driver RtkUsbCdcAcmSetup.INF, and before the installation, please make sure the VID/PID value used in CDC ACM class is included in the INF file:
```
[DeviceList]
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8720
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8721
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8722
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8730
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8006
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8061
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_F851
; Add support for new VID/PID
 [DeviceList.NTamd64]
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8720
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8721
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8722
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8730
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8006
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8061
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_F851
; Add support for new VID/PID
```

# Supported IC

RTL8730E
RTL8721Dx
RTL8721F
