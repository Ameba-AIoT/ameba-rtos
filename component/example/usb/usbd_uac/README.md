# Example Description

In this application, Ameba is designed as a UAC 2.0 device, USB host (e.g. Windows PC) can recognize Ameba as an audio device with speaker, then communicates with it via USB interface.

# HW Configuration

None

# SW configuration

1. Menuconfig
	Type command `./menuconfig.py` under the project directory and :
	- Choose `CONFIG USB --->`:
	```
	[*] Enable USB
			USB Mode (Device)  --->
	- Choose UAC version 1.0 or 2.0 :
	[*] UAC
			Select UAC Version (UAC 2.0)  --->
	```
	- Choose `CONFIG APPLICATION --->` -> `Audio Config --->`:
	```
	[*] Enable Audio Framework
			Select Audio Interfaces (Mixer)  --->
	```
	Save and exit.

2. Build
	Type command `./build.py -a usbd_uac` under the project directory to build images.

3. Download
	Download images to board by Ameba Image Tool.

# Expect result

1. Plugin Reset the board, following log shall be printed on the LOGUART console:
	```
	[UAC-I] USBD uac demo start
	```

2. Connect the USB port of Ameba board to USB UAC host (for example PC) with USB cable.

3. Reset and check the log via LOGUART console, USB Audio loading log will be printed, make sure there is no error reported.

4. After the USB Audio driver is successfully loaded, USB host end will recognize Ameba as an audio device.

5. Now user can play audio via the audio device.

# Note

None

# Supported IC

- RTL8721Dx
- RTL8730E
