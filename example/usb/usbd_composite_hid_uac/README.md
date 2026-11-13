# Example Description

In this application, Ameba is designed as a USB Type-C headset: a USB UAC and HID composite device. The USB host recognizes Ameba as an audio speaker (and a microphone with UAC 2.0) plus the standard headset buttons on the HID Consumer Control page, Play/Pause, Volume Up/Down, Voice Command and Mute, as the Android USB headset specification defines. They work with the inbox drivers of Windows, macOS, Linux, Android and iOS, no vendor tool is needed.

The interfaces are laid out as on a Type-C headset: the UAC interfaces come first, the HID button interface last.

# HW Configuration

None

# SW configuration

1. Menuconfig
	Use menuconfig and :
	- Choose `CONFIG USB --->`:
	```
	[*] Enable USB
			USB Mode (Device)  --->
		*** USB Device Global Settings ***
	[*] Composite function enable
		*** USB Device Class Selection ***
	[*] HID
			Select HID device type (Headset buttons)  --->
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
	[COMP-I] UAC stop 0
	```
	The reported channel count, sample rate and bit width shall match the format selected on the host side. The playback data path itself prints no log. Every mute/volume change issued by the host prints `[COMP-I] Mute:<0|1> vol:<value>`.

	Play music on a PC, if you connect a speaker to the board, sound will come out
   ```
	- Open the OS Sound Bar, enable the Realtek HID+UAC Composite Device speaker device if it is disabled. Locate the device in the Device Manager and enable it manually.
	- Open the OS Sound Bar, set Realtek HID+UAC Composite Device Speaker as default device.
	- Open the Sound Control Panel, select the Realtek HID+UAC Composite Device Speaker -- configure the speakers -- choose a specific channel setup -- finalize the configuration, and then click Test.
	- If you connect a speaker to the board, sound will play.
   ```

	UAC2.0 supports audio recording by default. The following guide demonstrates how to capture and verify the recording data path using Windows 11 and Audacity as the test environment.
	```
	- Configure Recording Software: Launch Audacity. Navigate to Audio Setup > Recording Device and select the target device: Microphone (Realtek HID+UAC Composite Device).
	- Initialize Data Stream: Click the Record button on the Audacity toolbar to prepare the Host to receive audio frames.
	- Execute Test Command: In the device's serial console (UART), execute "AT+USBDHIDUAC=record" command to trigger data transmission.
	- Verify Output: Audacity will begin receiving a continuous, looping data stream. Once sufficient data has been captured, click the Stop button and play back the track to verify audio integrity; a clear, looping audio signal should be heard.
	```
	Triggering the record command prints:
	```
	[COMP-I] UAC record start
	```

5. Headset button check: type the commands below in the LOGUART console, each clicks one button (press, then release), the host reacts as to the buttons of a Type-C headset:
	```
	AT+USBDHIDUAC=key,volup      # Volume Up: the OS volume bar pops up and the volume goes up
	AT+USBDHIDUAC=key,voldown    # Volume Down
	AT+USBDHIDUAC=key,mute       # Mute toggle
	AT+USBDHIDUAC=key,play       # Play/Pause the media player, on a phone it also answers / hangs up a call
	AT+USBDHIDUAC=key,voice      # Voice Command: launches the voice assistant on Android, ignored by most PCs
	```
	A headset that lacks a button just never reports it, the host needs nothing more.

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
| `AT+USBDHIDUAC=key,<play\|volup\|voldown\|voice\|mute>` | Click one headset button: report it pressed, then released. `play`: Play/Pause, `volup`/`voldown`: Volume Up/Down, `voice`: Voice Command, `mute`: Mute. |
| `AT+USBDHIDUAC=record` | Start the microphone (USB IN) replay, which feeds the built-in audio sample to the host. Each call queues one more session. Only on a UAC 2.0 and non-FS-only build. |

Note: `record` is only available on a UAC 2.0 and non-FS-only build.

# Note

The example needs the HID device type `Headset buttons` (`CONFIG_USBD_HID_HEADSET`), the build stops with an error otherwise. For a vendor defined HID protocol refer to the `usbd_hid` example instead.

Windows caches the descriptors of a device per VID/PID. After a firmware update that changes the UAC or HID descriptors (e.g. channel count, microphone enabled or not), uninstall the device in Device Manager (with its driver) and replug it, otherwise the host may keep using the old layout and the audio endpoints do not work.

# Supported IC

RTL8730E
RTL8721Dx
RTL8721F
