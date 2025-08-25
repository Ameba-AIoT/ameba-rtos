# Example Description

With USB host v2.0 interface, Ameba can be designed to a USB vendor-specific UAC host.

In this application, Ameba boots up as USB host which can communicate with USB vendor-specific UAC device as defined in `usbd_uac` application.

Only for Reaktek RD internal usage.

# HW Configuration

Some Ameba boards need to be reworked to support USB host, refer to AN for details.
Taking RTL8730EA/QFN100 boards for example, the register R20 shall be parted on with 0Ohm register or just shorted to provide power for USB device via VBUS.

# SW configuration

1. Pre-configuration
   Adjust `USBH_UAC_INTR_BUF_SIZE` and `USBH_UAC_ISOC_BUF_SIZE` to sync with the settings of USB vendor-specific UAC device.

2. Menuconfig
	Type command `./menuconfig.py` under the project directory:
	- Choose `CONFIG USB --->`:
	```
	[*] Enable USB
			USB Mode (Host)  --->
	[*] 	UAC
	```
	- Choose `CONFIG VFS --->`:
	```
	[*] Enable VFS FATFS
	```
	Save and exit.

3. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

USB UAC specific log will be printed on loguart console, make sure there is no error reported.
```
[UAC-I] USBH UAC demo start
```

# Note

Please contact with USB SW RD.

# Supported IC

- RTL8730E
