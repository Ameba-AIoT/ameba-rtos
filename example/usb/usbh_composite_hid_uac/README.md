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

1. Menuconfig
	Use menuconfig and :
	- Choose `CONFIG USB --->`:
	```
	[*] Enable USB
			USB Mode (Host)  --->
	[*] Composite
			Select Composite Class (HID + UAC)  --->
				(X) HID + UAC
					Select UAC Version (UAC 1.0)  --->
	Save and exit.

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

1. Reset the board, following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
	```
	[COMP-I] USBH HID+UAC comp demo start
	```

2. Connect the USB port of Ameba board to USB UAC HID device (for example headphone) with USB cable.

3. Once the USB audio driver enumerates successfully, Ameba recognizes the headset and brings up both audio pipelines.

	- **Playback**: Put on the headset. A built-in 1-second PCM data plays back in a loop. Per-loop playback statistics are reported on the console:
		```
		[COMP-I] Play stats loop=11 err=0 busy=0
		[COMP-I] Play stats loop=12 err=0 busy=0
		```

	- **Recording**: If the headset exposes a microphone, connect a speaker to the board's audio output, then speak into the headset microphone. The captured PCM is rendered through the speaker output, and per-batch recording statistics are reported on the console:
		```
		[COMP-I] Rec stats loop=1225 bytes=1058400 err=0
		[COMP-I] Rec stats loop=1334 bytes=1152576 err=0
		```

4. If the headset also has HID media keys, each press is printed and acted on. Holding a key acts once, not repeatedly.

	| Key | Action |
	| --- | --- |
	| Volume Up / Down | Step the volume of the selected stream (see `AT+USBHUAC=init`) by `CONFIG_USBH_COMP_HID_UAC_VOLUME_STEP` (20 by default), as `AT+USBHUAC=vol,up\|down` does |
	| Mute | Toggle mute of the selected stream, from the mute state the device reports |
	| Play/Pause | Pause playback, press again to resume where it stopped |
	| Stop | Stop playback, Play/Pause then restarts it from the beginning |

	```
	Volume Up
	[COMP-I] Vol 70
	Play/Pause
	[COMP-I] Play paused
	Play/Pause
	[COMP-I] Play resumed
	```

	A paused playback stops the ISOC OUT stream. A volume or mute key prints `Set vol fail` / `Set mute fail` when the device has no such control. A newly attached device starts playing; its mute state is left as the device keeps it.

5. Reset and check the log via LOGUART console, make sure there is no error reported


# Shell commands

Type the AT commands on the LOGUART console. Parameters are comma separated; the command name is case-sensitive, the parameters are not. `AT+<cmd>=help` prints the usage, every command ends with `OK` or `ERROR: <code>`.

| Command | Description |
| --- | --- |
| `AT+USBHUAC=init[,<play\|record>]` | Select the stream the following `mute`/`vol` act on, default `play`. |
| `AT+USBHUAC=mute,<0\|1>` | Mute (`1`) or unmute (`0`) the selected stream. |
| `AT+USBHUAC=vol,<0..100\|up\|down>` | Set the volume of the selected stream in percent, or step it by 20. Each stream starts at 50. |
| `AT+USBHUAC=hotplug` | Power-cycle the device through the power-control GPIO, to exercise re-enumeration. Prints `Not support` unless `CONFIG_USBH_COMP_HID_UAC_GPIO_POWER_CTRL` is set. |

`mute`/`vol` need an attached and set up device.

# Supported IC

RTL8730E
RTL8721F
