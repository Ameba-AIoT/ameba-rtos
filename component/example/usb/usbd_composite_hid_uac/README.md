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
	[*] Composite
			Select Composite Class (HID + UAC)  --->
	- Choose UAC Version 1.0 or 2.0 :
	(X) HID + UAC
			Select UAC Version (UAC 2.0)  --->
	```
	- Choose `CONFIG APPLICATION --->` -> `Audio Config --->`:
	```
	[*] Enable Audio Framework
			Select Audio Interfaces (PassThrough)  --->
		
	Note: If the audio speaker is not very smooth, choose `Select Audio Interfaces (Mixer)`, check the audio composite note to get more details.
	```
	Save and exit.

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

1. Reset the board, following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
	```
	[COMP-I] USBD HID_UAC demo start
	```

2. Connect the USB port of Ameba board to USB UAC HID host (for example PC) with USB cable.

3. After the USB Audio driver is successfully loaded, USB host end will recognize Ameba as an audio device.

4. Play music on a PC, if you connect a speaker to the board, sound will come out
   ```
	- Open the OS Sound Bar, enable the Realtek Composite Device speaker device if it is disabled. Locate the device in the Device Manager and enable it manually.
	- Open the OS Sound Bar, set Realtek Composite Device Speaker as default device.
	- Open the Sound Control Panel, select the Realtek Composite Device Speaker -- configure the speakers -- choose a specific channel setup -- finalize the configuration, and then click Test.
	- If you connect a speaker to the board, sound will play.
   ```

5. Use HIDPyToy tool to test the HID message(download address https://github.com/todbot/hidpytoy)
   	```
	- Connect the HID device
		click rescan ,choose "Realtek HID Priv Device" device,and click connect
	- Test
		USBD RX:type "0x1F,1,2,3,4" in the HIDPyToy,and click "Send Out Report", the LOGUART console will print the message
		USBD TX:type "usbd tx" in the LOGUART console, and click "Read In Reports" in the HIDPyToy, you will get a message from the ameba device
		Notice:please trigger USBD RX first
	```

6. Type "usbd vol1"/"usbd vol2" in the LOGUART console, the windows volume control bar will pop-up, and the volume will up/down

7. Reset and check the log via LOGUART console, make sure there is no error reported
   

# Supported IC

RTL8730E
RTL8721Dx
