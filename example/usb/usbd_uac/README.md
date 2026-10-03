# Example Description

In this application, Ameba is designed as a UAC 2.0 device, USB host (e.g. Windows PC) can recognize Ameba as an audio device with speaker, then communicates with it via USB interface.

# HW Configuration

None

# SW configuration

1. Menuconfig
	Use menuconfig and :
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

	Note: For details on audio configuration, please refer to https://aiot.realmcu.com/en/latest/rtos/multimedia/audio/index.html
	```

	If the development board model is RTL8721F, please follow the steps below to select FREERTOS via menuconfig.
	Use menuconfig and :
	- Choose `CONFIG OS --->`:
	```
	[*] Enable Kernel
			Kernel Selecte (FREERTOS)--->
	```
	Save and exit.

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

1. Reset the board, following log shall be printed on the LOGUART console. The test PASSes only if the lines below show up and no error level log (`-E`) is reported:
	```
	[USBD-A] INIT
	[UAC-I] USBD UAC demo start
	[UAC-I] ATTACHED
	```
	Notes:
	* `[USBD-A] INIT` comes from the USB device core, not from the example.
	* `[UAC-I] ATTACHED` is printed once the USB host has enumerated the device, so it only shows up after the cable is connected.
	* On AmebaGreen2 an extra `[USB-I] UPHY para from ...` line is printed by the USB HAL before `[USBD-A] INIT`.

2. Connect the USB port of Ameba board to USB UAC host (for example PC) with USB cable.

3. Once the host selects an audio format and starts the stream, the playback worker brings up the audio track and following log is printed:
	```
	[UAC-I] Audio track demo begin
	[UAC-I] Audio ch:2,rate:48000,bits=16
	[UAC-I] Track buf size:<value>
	[UAC-I] UAC stop 0
	```
	The reported channel count, sample rate and bit width shall match the format selected on the host side. The playback data path itself prints no log. Every time the host switches the format, `[UAC-I] Audio track demo stop` is printed and the four lines above are printed again for the new format. On a build without the audio framework (`CONFIG_SUPPORT_AUDIO_FOR_USB` disabled) the audio track is not created, and a `[UAC-I] Audio track get <len> <total>` line is printed every 200 reads instead.

4. After the USB Audio driver is successfully loaded, USB host end will recognize Ameba as an audio device. The device is named `Realtek UAC2.0 Device` on a UAC 2.0 build and `Realtek UAC1.0 Device` on a UAC 1.0 build, the steps below use the UAC 2.0 name.
   ```
	- Open the OS Sound Bar, enable Realtek UAC2.0 Device speaker device if it is disabled. Locate the device in the Device Manager and enable it manually.
	- Open the OS Sound Bar, set Realtek UAC2.0 Device Speaker as default device.
	- Open the Sound Control Panel, select the Realtek UAC2.0 Device Speaker device -- configure the speakers -- choose a specific channel setup -- finalize the configuration, and then click Test.
	- If you connect a speaker to the board, sound will play.
   ```

5. Now user can play audio via the audio device.

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
| `usbd_uac_record` | Start the microphone (USB IN) replay, which feeds a synthetic 1 kHz tone to the host. Prints `[UAC-I] UAC record start`. |

Note: recording is only wired up in the UAC 2.0 class driver, so the command is only registered on a UAC 2.0 and non-FS-only build.

# Note

None

# Supported IC

RTL8730E
RTL8721Dx
RTL8721F
