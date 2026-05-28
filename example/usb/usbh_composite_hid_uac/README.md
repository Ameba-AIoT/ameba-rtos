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
	[COMP-I] USBH UAC&HID composite demo start
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

4. If the headphones also have volume control buttons, when you press the button, you will receive the following print on the serial port
   	```
	- Volume Up
	- Volume Down
	```

5. Reset and check the log via LOGUART console, make sure there is no error reported


# Supported IC

RTL8730E
RTL8721F
