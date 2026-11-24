# Example Description

With USB host v2.0 interface, Ameba can be designed to a standard USB Audio Class 1.0 (UAC 1.0) host.

In this application, Ameba boots up as USB host which communicates with a UAC 1.0 device, such as a USB headset or another Ameba board running the `usbd_uac` application built as UAC 1.0:
- Playback (USB ISOC OUT): an embedded PCM clip (`example_usbh_uac_audio_data.h`, 2 channels / 16 bit / 48 kHz) is streamed to the device in a loop. For an asynchronous device with a feedback endpoint, such as `usbd_uac` with `CONFIG_USBD_UAC_PLAY_FEEDBACK`, the feedback is polled and the packet size follows the device rate.
- Record (USB ISOC IN): if the device has a microphone, the first advertised IN format is selected and the captured data is read continuously. On SoCs with `CONFIG_SUPPORT_AUDIO_FOR_USB` (RTL8730E) the captured audio is also rendered on the local audio output through the audio framework; on other SoCs it is read and dropped.
- Hot-plug: on every detach the host stack is torn down and re-initialized, so a re-plugged device runs again without a board reset.

Only for Reaktek RD internal usage.

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

1. Pre-configuration
   The following macros in `example_usbh_uac.c` define the stream setup:
   * `USBH_UAC_PLAY_CHANNELS` / `USBH_UAC_PLAY_BITWIDTH` / `USBH_UAC_PLAY_SAMPLING_FREQ` (default `2` / `16` / `48000`): playback format. It must match the embedded PCM data and be supported by the device, else `Play set alt fail` is reported and no playback runs.
   * `USBH_UAC_FRAME_CNT` (default `20`): ring buffer depth in USB frames for each ISOC direction.
   * `USBH_UAC_HOT_PLUG_TEST` (default `1`): re-initialize the stack on every detach. Set to `0` to stop playback after `USBH_UAC_TEST_CNT` (60) loops.

2. Default configuration

   The `prj.conf` of this example sets the options below by default, no menuconfig is needed to run it:
   ```
   CONFIG USB --->
   [*] Enable USB
           USB Mode (Host)  --->
   [*]     UAC
   ```

3. Build

   ```
   ./ameba.py build -a usbh_uac -p
   ```
   `-p` (pristine) drops the previous configuration and applies the `prj.conf` of this example.

4. (Optional) Change the configuration

   Only needed when the default configuration does not fit:
   1. Build once with `-p` as in step 3, so that the configuration starts from `prj.conf`.
   2. Run `./ameba.py menuconfig` and change the options.
   3. Rebuild without `-p`: `./ameba.py build -a usbh_uac`.

   To keep a change permanently, add its `CONFIG_XXX=y` lines to `prj.conf` instead and build with `-p`.

5. Download

   Flash the images to the board with any of the tools below:
   * VS Code extension: https://aiot.realmcu.com/en/latest/tools/vscode/index.html
   * Image Tool (Windows): https://aiot.realmcu.com/en/latest/tools/image_tool/index.html
   * Flash Tool (cross-platform): https://aiot.realmcu.com/en/latest/tools/flash/index.html
   * Web flash tool: https://aiot.realmcu.com/en/resource/web_flash_tool.html

6. Prepare the UAC device: a UAC 1.0 USB headset/speaker supporting 2 channels / 16 bit / 48 kHz playback, or another Ameba board running the `usbd_uac` example with `Select UAC Version (UAC 1.0)`.

# Expect result

1. Reset the board, the following log shall be printed on the LOGUART console:
	```
	[UAC-I] USBH UAC demo start
	[UAC-I] Play wait setup
	[UAC-I] Rec wait setup
	[UAC-I] INIT
	```
	The order of these lines may vary.

2. Connect the UAC device to the USB port of the board. After enumeration the class dumps the device capabilities, then the play (and record, if the device has a microphone) thread starts:
	```
	[UAC-I] ATTACH
	[UAC-I] SETUP
	[UAC-I] Play fmt list:
	[UAC-I] [0] CH=2 BW=16 FREQ=48000
	[UAC-I] Play start, data=<len> frame=<size> test_cnt=60
	[UAC-I] Rec fmt list:
	[UAC-I] [0] CH=1 BW=16 FREQ=48000
	[UAC-I] Rec start, frame=<size>
	[UAC-I] Play status loop=1 err=0
	[UAC-I] Rec status loop=<n> bytes=<n> empty=0
	...
	```
	A format with a continuous rate range (`bSamFreqType` 0) takes one line printed as `FREQ=<min>-<max>`, e.g. `[0] CH=2 BW=16 FREQ=8000-48000`; any rate within it can be selected.
	On SoCs with `CONFIG_SUPPORT_AUDIO_FOR_USB`, `Rec init ch=... rate=... bits=...`, `Rec track buf <n>` and `Rec track started` are printed before `Rec start`. A device without microphone prints `Rec not support`; a device without speaker prints `Play not support`.

	The test PASSes when the clip is heard on the device, `Play status` and `Rec status` (if recording) keep printing every 10 s with `loop` increasing, and sooner (at most once per second) whenever `err` or `empty` grows, and no `[UAC-E]` log is reported. The `err` counter counts ISOC OUT errors and full-ring back-offs, `empty` counts reads that returned no data within 1 s.

3. Unplug the device. The detach log appears and the hotplug thread reinitializes the host stack:
	```
	[UAC-I] DETACH play=<n> rec=<n>
	[UAC-I] Play stop loops=<n>
	[UAC-I] Rec stop loops=<n> bytes=<n>
	[UAC-I] ========== Hotplug #1 ==========
	[UAC-I] DEINIT
	[UAC-I] Free heap: 0x...
	[UAC-I] INIT
	[UAC-I] Reinit done
	```
	The order of these lines may vary. Re-plug the device and step 2 repeats without a board reset. If the stack cannot be rebuilt, `[UAC-E] Reinit fail <err>` and `[UAC-E] Hotplug thread exit` are printed and the board must be reset.

	Typical failure logs: `Probe Fail`, `Play get fmt fail`, `Play set alt fail`, `Rec get fmt fail`, `Rec set alt fail`, `Play write err <err>`, `Rec read err <err>`.

# Shell commands

Type the AT commands on the LOGUART console. Parameters are comma separated; the command name is case-sensitive, the parameters are not. `AT+USBHUAC=help` (or `AT+USBHUAC=?`) prints the usage, every command ends with `OK` or `ERROR: <code>`.

| Command | Description |
| --- | --- |
| `AT+USBHUAC=help` | Print the usage. |
| `AT+USBHUAC=mute,<0\|1>` | Mute (`1`) or unmute (`0`) the playback (USB OUT) stream. |
| `AT+USBHUAC=vol,<0..100\|up\|down>` | Set the playback volume in percent, or step it by 5. Starts at 50; the value is only kept when the device accepted it and is then printed as `[UAC-I] Vol <n>`. |

Errors:
- `[UAC-E] Not running`: the demo is not running.
- `[UAC-E] Stack not ready`: the USB host stack is not up (e.g. during a hotplug re-init).
- `[UAC-E] Set fail <err>`: no device is set up, or the device refused or does not support the control.

# Note

- The device must accept the playback format set by `USBH_UAC_PLAY_xxx`. For the record direction the first advertised IN format is used.
- For other chips, refer to the AN for setup guide.

# Supported IC

RTL8730E
RTL8721F
