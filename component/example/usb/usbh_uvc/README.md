# Example Description

In this application, Ameba is designed as an USB UVC host which can communicate with specific USB camera.

# HW Configuration

Some Ameba boards need to be reworked to support USB host, refer to AN for details.
Taking RTL8730EA/QFN100 boards for example, the register R20 shall be parted on with 0Ohm register or just shorted to provide power for USB device via VBUS.

# SW configuration

1. Configure test scenarios
	Configure `CONFIG_USBH_UVC` in `example_usbh_uvc.c` for different test scenarios:
	- `USBH_UVC_SIMPLE`: Capture the frame and abandon it
	- `USBH_UVC_VFS`: Capture the frame and save it to filesystem. User need to configure VFS correctly refer to VFS example or document.
	- `USBH_UVC_HTTPC`: Capture the frame and save it to http server.

	Configure CONFIG_USBH_UVC_FORMAT in example_usbh_uvc.c to test different video frame formats:
	- `USBH_UVC_FORMAT_MJPEG`: Test capturing and processing MJPEG compressed frames.
	- `USBH_UVC_FORMAT_YUV`: Test capturing and processing uncompressed YUV frames (Raw Data).
	- `USBH_UVC_FORMAT_H264`: Test capturing and processing H.264 (AVC) encoded frames.

2. Menuconfig
	Type command `./menuconfig.py` under the project directory and choose `CONFIG USB`:
	```
	[*] Enable USB
			USB Mode (Host)  --->
	[*] UVC
	```
	Save and exit.

3. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

1. Attach an USB camera to the USB port of the Ameba board.
2. Reset the board, following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
	```
	[UVC-I] USBH UVC demo start
	```

3. Ameba board will recoganize the USB camera and start to capture vedio frames at the rate of one frame per second.

4. Once attach uvc camera to host, host will capture 200 images cyclically. Following log shall be printed on the LOGUART console:
	```
	[UVC-I] USBH UVC demo start
	[MAIN-I] KM4 START SCHEDULER
	...
	[UVC-I] INIT
	[USBH-A] Device connect
	[USBH-A] PID: 0x9230
	[USBH-A] VID: 0x05a3
	[USBH-A] Cfg 1
	[USBH-A] Address 1 assigned
	[USBH-A] MFG: HD Camera Manufacturer
	[USBH-A] PROD: USB 2.0 Camera
	[USBH-A] SN: N/A
	[USBH-A] Enum done, total 1 cfg
	[UVC-I] ATTACH
	[UVC-I] Set paras: MJPEG 1280*720@60fps
	[UVC-I] Stream on
	[UVC-I] Captured frame 0, len=20832
	[UVC-I] Captured frame 1, len=20912
	...
	[UVC-I] Captured frame 199, len=108264
	[UVC-I] TP 4126 KB/s @ 4953 ms, fps 40/s
	[UVC-I] TP 4.0 MB/s-40 (0_20930888/200)
	[UVC-I] Stream off
	[UVC-I] USBH UVC demo stop
# Note

None

# Supported IC

RTL8730E
RTL8721F
