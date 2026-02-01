# Example Description

In this application, Ameba is designed as a USB UAC and HID composite host, It can connect to UAC and HID composite devices such as headphones. Ameba will send audio data to the headphone, If the button in the headphone is pressed, Ameba will recognize the command.

# HW Configuration

None

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

3. After the USB Audio driver is successfully loaded, Ameba will recognize the headphone.

   ```
	- When you put on the headphones, there will be sound output here, with a 1-second sound loop output of 30 times.
   ```

4. If the headphones also have volume control buttons, when you press the button, you will receive the following print on the serial port
   	```
	- Executing Volume Up
	- Executing Volume Down
	```

5. Reset and check the log via LOGUART console, make sure there is no error reported


# Supported IC

RTL8730E
RTL8721F
