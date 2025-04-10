# Example Description

In this application, Ameba is designed as a USB UAC and HID composite device, USB host can recognize Ameba as a audio speaker and a HID vendor. 

# HW Configuration

None

# SW configuration

1. Menuconfig
	Type command `./menuconfig.py` under the project directory:
	- choose `CONFIG USB --->`:
		```
		[*] Enable USB
				USB Mode (Device)  --->
		[*] 	Composite
					 Select Composite Class (HID + UAC)  --->
		```
	- choose `CONFIG APPLICATION --->`:
		```
		Audio Config
			[*] Enable Audio Framework
					 Select Audio Interfaces (Mixer)  --->
		```
	Save and exit.

2. Build
	Type command `./build.py -a usbd_composite_uac_hid` under the project directory to build images.

3. Download
	Download images to board by Ameba Image Tool.

# Expect result

1. Reset the board, following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
	```
	[COMP-I] USBD HID_UAC demo start
	```

2. Connect the USB port of Ameba board to USB UAC HID host (for example PC) with USB cable.
3. Play music on a PC, if you connect a speaker to the board, sound will come out
4. Use HIDPyToy tool to test the HID message(download address https://github.com/todbot/hidpytoy)
   	```
	- Connect the HID device
		click rescan ,choose "Realtek HID Priv Device" device,and click connect
	- Test
		USBD RX:type "0x1F,1,2,3,4" in the HIDPyToy,and click "Send Out Report", the LOGUART console will print the message
		USBD TX:type "usbd tx" in the LOGUART console, and click "Read In Reports" in the HIDPyToy, you will get a message from the ameba device
		Notice:please trigger USBD RX first
	```
5. Type usbd vol1/vol2 in the LOGUART console, the windows volume control bar will pop-up, and the volume will change
6. Reset and check the log via LOGUART console, make sure there is no error reported
   

# Supported IC

- RTL8730E
- RTL8721Dx
- RTL8711Dx