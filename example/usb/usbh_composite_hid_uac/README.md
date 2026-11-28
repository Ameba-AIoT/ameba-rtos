# Example Description

In this application, Ameba is designed as a USB UAC and HID composite host, It can connect to UAC and HID composite devices such as headphones. Ameba will send audio data to the headphone, If the button in the headphone is pressed, Ameba will recognize the command.

# HW Configuration

Some Ameba boards require hardware rework to support USB Host mode.
Populate the designated resistor footprint with a **0Ohm resistor** (or apply a solder bridge)
to enable VBUS power supply to the USB device.
Refer to the [EVB User Guide](https://aiot.realmcu.com/filelist?document_type=9) for details.

## Board Rework Reference

### RTL8730E

| Board Variant      | Resistor | Rework Action                                  |
|--------------------|----------|------------------------------------------------|
| RTL8730EA / QFN100 | R20      | Populate with 0Ohm resistor or solder bridge     |
| RTL8730EL / QFN144 | R166     | Populate with 0Ohm resistor, or use an OTG cable |

### RTL8721F

| Board Variant    | Resistor | Rework Action                              |
|------------------|----------|--------------------------------------------|
| RTL8721FC0-R00   | R52      | Populate with 0Ohm resistor                 |
| RTL8721FA0-3V0   | R52      | Populate with 0Ohm resistor                 |
| RTL8721FL0-2V0   | R2       | Populate with 0Ohm resistor *(pre-reworked by default)* |

# SW configuration

1. Default configuration

	The `prj.conf` of this example sets the options below by default, no menuconfig is needed to run it:
	```
	CONFIG USB --->
	[*] Enable USB
			USB Mode (Host)  --->
	[*] UAC
	[*] HID
	```

2. Build

	```
	./ameba.py build -a usbh_composite_hid_uac -p
	```
	`-p` (pristine) drops the previous configuration and applies the `prj.conf` of this example.

3. (Optional) Change the configuration

	Only needed when the default configuration does not fit:
	1. Build once with `-p` as in step 2, so that the configuration starts from `prj.conf`.
	2. Run `./ameba.py menuconfig` and change the options.
	3. Rebuild without `-p`: `./ameba.py build -a usbh_composite_hid_uac`.

	To keep a change permanently, add its `CONFIG_XXX=y` lines to `prj.conf` instead and build with `-p`.

	The composite host is built at the application level by registering the standard UAC and HID class drivers to the host core together, there is no separate composite option.

4. Download

	Flash the images to the board with any of the tools below:
	* VS Code extension: https://aiot.realmcu.com/en/latest/tools/vscode/index.html
	* Image Tool (Windows): https://aiot.realmcu.com/en/latest/tools/image_tool/index.html
	* Flash Tool (cross-platform): https://aiot.realmcu.com/en/latest/tools/flash/index.html
	* Web flash tool: https://aiot.realmcu.com/en/resource/web_flash_tool.html

# Device requirements

* A USB headset with a UAC 1.0 / 2.0 audio function and, for the key test, an HID consumer-control interface.
* Playback: the headset shall offer a 2 channel, 16 bit, 48 kHz playback format, otherwise `[COMP-E] Play set alt fail` is printed and nothing is played.
* Recording: the first microphone format the headset lists is used, whatever it is.
* Speaker output of the recording: only on SoCs with `CONFIG_SUPPORT_AUDIO_FOR_USB` (RTL8730E). The captured PCM is then rendered through the audio output of the board; on the other SoCs the data is read and dropped.

# Expect result

1. Reset the board, following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
	```
	[COMP-I] USBH HID+UAC comp demo start
	[COMP-I] Play wait setup
	[COMP-I] Rec wait setup
	[COMP-I] INIT
	```
	The worker threads print their lines on their own, so the order may vary.

2. Connect the USB port of Ameba board to USB UAC HID device (for example headphone) with USB cable.

3. Once the device enumerates successfully, Ameba lists the formats of the headset and brings up both audio pipelines:
	```
	[COMP-I] ATTACH
	[COMP-I] SETUP
	[COMP-I] Play fmt list:
	[COMP-I] [0] CH=2 BW=16 FREQ=48000
	[COMP-I] Rec fmt list:
	[COMP-I] [0] CH=1 BW=16 FREQ=48000
	[COMP-I] Play start, data=192000 frame=<n>
	[COMP-I] Rec start, frame=<n>
	```
	A format with a continuous rate range (`bSamFreqType` 0) takes one line printed as `FREQ=<min>-<max>`, e.g. `[0] CH=2 BW=16 FREQ=8000-48000`; any rate within it can be selected.

	- **Playback**: Put on the headset. A built-in 1-second PCM data (2 channel, 16 bit, 48 kHz) plays back in a loop. Playback statistics are reported on the console every 10 s, and sooner (at most once per second) whenever `err` grows; `err` counts the writes the device did not take in time:
		```
		[COMP-I] Play status loop=11 err=0
		[COMP-I] Play status loop=21 err=0
		```

	- **Recording**: If the headset exposes a microphone, speak into it. On RTL8730E connect a speaker to the board's audio output first: the captured PCM is rendered through it (`[COMP-I] Rec init ch=<n> rate=<n> bits=<n>` and `[COMP-I] Rec track started` are printed before `Rec start`). Recording statistics are reported on the console every 10 s, and sooner (at most once per second) whenever `empty` grows; `empty` counts the reads that got no data within 1 s:
		```
		[COMP-I] Rec status loop=1225 bytes=1058400 empty=0
		[COMP-I] Rec status loop=1334 bytes=1152576 empty=0
		```
		A headset without microphone prints `[COMP-I] Rec not support`.

4. If the headset also has HID media keys, each press is printed and acted on. Holding a key acts once, not repeatedly.

	| Key | Log | Action |
	| --- | --- | --- |
	| Volume Up / Down | `Volume Up` / `Volume Down`, then `[COMP-I] Vol <0..100>` | Step the volume of the selected stream (see `AT+USBHHIDUAC=init`) by 20 (`CONFIG_USBH_COMP_HID_UAC_VOLUME_STEP`, a `#define` in the example), as `AT+USBHHIDUAC=vol,up\|down` does. Each stream starts at 50 |
	| Mute | `Mute`, then `[COMP-I] Muted` / `[COMP-I] Unmuted` | Toggle mute of the selected stream, from the mute state the device reports |
	| Play/Pause | `Play/Pause`, then `[COMP-I] Play paused` / `[COMP-I] Play resumed` | Pause playback, press again to resume where it stopped |
	| Stop | `Stop`, then `[COMP-I] Play paused` | Stop playback, Play/Pause then restarts it from the beginning |

	```
	Volume Up
	[COMP-I] Vol 70
	Play/Pause
	[COMP-I] Play paused
	Play/Pause
	[COMP-I] Play resumed
	```

	A paused playback stops the ISOC OUT stream. A volume or mute key prints `[COMP-E] Set vol fail: <err>` / `[COMP-E] Set mute fail: <err>` when the device has no such control. A newly attached device starts playing; its mute state is left as the device keeps it.

5. Hotplug check: the example tears down and re-inits the USB stack on every detach, so unplug and re-plug the headset, following log shall be printed for each cycle, followed by `Play wait setup` / `Rec wait setup` and the log of step 3 (lines of different threads may interleave):
	```
	[COMP-I] DETACH played=<n> rec=<n>
	[COMP-I] Play stop, loops=<n>
	[COMP-I] Rec stop, loops=<n> bytes=<n>
	[COMP-I] 
	========== Hotplug #1 ==========
	[COMP-I] DEINIT
	[COMP-I] Free heap: 0x<value>
	[COMP-I] INIT
	[COMP-I] Reinit done
	```
	A `[COMP-W] Play write err <n>` or `[COMP-W] Rec read err <n>` right after `DETACH` is expected: the stream is stopped by the detach. The `Free heap` value shall stay stable across cycles, a value that keeps dropping indicates a memory leak. When the host core resets or power-cycles the device by itself (e.g. after a transfer error, or `AT+USBHHIDUAC=hotplug`), the stack is kept: `DETACH` is followed by `[COMP-I] Reset, stack kept` and the log of step 3, without `DEINIT`/`Free heap`/`INIT`.

6. Make sure there is no error reported during the whole test.

# Shell commands

Type the AT commands on the LOGUART console. Parameters are comma separated; the command name is case-sensitive, the parameters are not. `AT+USBHHIDUAC=help` (or `AT+USBHHIDUAC=?`) prints the usage, every command ends with `OK` or `ERROR: <code>`. Except `help`, the commands are refused with `Not running` while the example is not running.

| Command | Description |
| --- | --- |
| `AT+USBHHIDUAC=init[,<play\|record>]` | Select the stream the following `mute`/`vol` act on, default `play`. Prints `Playback Control` or `Record Control`. |
| `AT+USBHHIDUAC=mute,<0\|1>` | Mute (`1`) or unmute (`0`) the selected stream. Prints `Mute` / `UnMute`, then reads the mute control of every channel back (`Verify mute(<dir>) ...` lines). |
| `AT+USBHHIDUAC=vol,<0..100\|up\|down>` | Set the volume of the selected stream in percent, or step it by 20. Each stream starts at 50. The volume of every channel is read back (`Verify vol(<dir>) ...` lines). |
| `AT+USBHHIDUAC=hotplug` | Have the host core power-cycle the device through VBUS (`usbh_dev_power_cycle()`), to exercise re-enumeration with the stack kept. Prints `[COMP-E] Not support` and `ERROR: -2` unless `CONFIG_USBH_COMP_HID_UAC_GPIO_POWER_CTRL` is set to 1. It is a `#define` in `example_usbh_composite_hid_uac.c` (default 0), not a Kconfig option; when set, the power-control GPIO `USB_DEV_PWR_CTRL_GPIO` (`_PA_3`) is registered as the VBUS switch of the host core (`usbh_cb_t::set_vbus`). |

`mute`/`vol` need an attached and set up device. Examples:
```
AT+USBHHIDUAC=init,play
AT+USBHHIDUAC=vol,80
AT+USBHHIDUAC=mute,1
AT+USBHHIDUAC=init,record
AT+USBHHIDUAC=vol,down
```

# Supported IC

RTL8730E
RTL8721F
