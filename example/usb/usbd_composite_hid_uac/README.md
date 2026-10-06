# Example Description

In this application, Ameba is designed as a USB UAC and HID composite device, USB host can recognize Ameba as a audio speaker and a HID vendor. 

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
			Select HID device type (Vendor bidirectional)  --->
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
	[COMP-I] USBD HID+UAC comp demo start
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
	- Execute Test Command: In the device's serial console (UART), execute "usbd_uac_record" command to trigger data transmission.
	- Verify Output: Audacity will begin receiving a continuous, looping data stream. Once sufficient data has been captured, click the Stop button and play back the track to verify audio integrity; a clear, looping audio signal should be heard.
	```
	Triggering the record command prints:
	```
	[COMP-I] UAC record start
	```

5. Use HIDPyToy tool to test the HID message(download address https://github.com/todbot/hidpytoy)
   	```
	- Connect the HID device
		click rescan ,choose "Realtek HID+UAC Composite Device" device,and click connect
	- Test
		USBD RX:type "0x1F,1,2,3,4" in the HIDPyToy,and click "Send Out Report", the LOGUART console will print the message
		USBD TX:type "usbd_hid_tx xx" in the LOGUART console, and click "Read In Reports" in the HIDPyToy, you will get a message from the ameba device
		Notice:please trigger USBD RX first
	```
	Each Out Report received from the host prints:
	```
	[COMP-I] HID RX <len> bytes, first byte:1f
	```

6. Type "usbd_hid_volup"/"usbd_hid_voldown" in the LOGUART console, the windows volume control bar will pop-up, and the volume will up/down

7. Hotplug check: the example tears down and re-inits the whole composite stack on every detach, so unplug and re-plug the cable, following log shall be printed for each cycle:
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
| `usbd_hid_tx [size]` | Send `size` bytes on the HID interrupt IN endpoint, default 10 bytes, clamped to the TX buffer length. |
| `usbd_hid_volup` | Send a HID consumer-control volume-up report to the host. |
| `usbd_hid_voldown` | Send a HID consumer-control volume-down report to the host. |
| `usbd_uac_record` | Start the microphone (USB IN) replay, which feeds the built-in audio sample to the host. Only registered on a UAC 2.0 and non-FS-only build. |

Note: the `usbd_hid_*` commands are only registered when the HID device type is `Vendor bidirectional` (`CONFIG_USBD_HID_BIDIR`).

# Supported IC

RTL8730E
RTL8721Dx
RTL8721F
