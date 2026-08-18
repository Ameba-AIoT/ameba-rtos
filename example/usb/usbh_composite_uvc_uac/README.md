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

1. Menuconfig
	Use menuconfig and:
	- Choose `CONFIG USB --->`:
	```
	[*] Enable USB
	        USB Mode (Host)  --->
	[*] UVC
	[*] UAC
	```
	Save and exit.

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

1. Reset the board, the following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
	```
	[COMP_UVC_UAC-I] USBH UVC&UAC composite demo start
	```

2. Connect the USB port of the Ameba board to a USB UVC+UAC composite device (e.g. a USB camera with built-in microphone) with a USB cable.

3. Once the device enumerates successfully, video streaming starts automatically. Frame statistics are reported on the console at 1-second intervals:
	```
	[COMP_UVC_UAC-I] UVC Attach
	[COMP_UVC_UAC-I] UAC Attach
	[COMP_UVC_UAC-I] UVC SETUP
	[COMP_UVC_UAC-I] UAC SETUP
	[COMP_UVC_UAC-I] UVC set param (2560x1440@30fps)
	[COMP_UVC_UAC-I] UVC streaming started
	[COMP_UVC_UAC-I] UVC frames: 30 (size=78643, ts=0)
	[COMP_UVC_UAC-I] UVC frames: 60 (size=78643, ts=0)
	```

4. If the device exposes a UAC microphone (ISOC IN), the audio capture pipeline starts automatically. The available audio formats are listed and the preferred format is selected. Audio data statistics are reported at 1-second intervals:
	```
	[COMP_UVC_UAC-I] Rec fmt list:
	[COMP_UVC_UAC-I] [0] CH=1 BW=16 FREQ=8000
	[COMP_UVC_UAC-I] [5] CH=1 BW=16 FREQ=48000
	[COMP_UVC_UAC-I] UAC rec started
	[COMP_UVC_UAC-I] Rec loop=94 bytes=96000
	[COMP_UVC_UAC-I] Rec loop=188 bytes=192000
	```
	If the device has no speaker output (ISOC OUT not supported), the following message is printed and the play thread exits silently:
	```
	[COMP_UVC_UAC-I] Play not ready or not supported
	```

5. Reset and check the log via LOGUART console, make sure there is no error reported.

# Supported IC

RTL8730E
RTL8721F
