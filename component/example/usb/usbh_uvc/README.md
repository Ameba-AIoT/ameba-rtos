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
	Use menuconfig and choose `CONFIG USB`:
	```
	[*] Enable USB
			USB Mode (Host)  --->
	[*] UVC
	```

	If Configure `USBH_UVC_VFS`:
	Type command `./menuconfig.py` under the project directory and choose `CONFIG VFS`:
	```
	[*] Enable VFS FATFS
			FATFS Memory Type (SD MODE)  --->
	```

	Save and exit.

3. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

For example, configure `USBH_UVC_SIMPLE` and `USBH_UVC_FORMAT_MJPEG`:

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

For example, configure `USBH_UVC_VFS` and `USBH_UVC_FORMAT_MJPEG`:

1. Insert a FAT32 formatted SD card into the SD card slot of the Ameba board.

2. Attach an USB camera to the USB port of the Ameba board .

3. Reset the board, following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
	```
	[UVC-I] USBH UVC demo start
	```

4. Ameba board will recognize the USB camera, create the file system thread, and start to capture video frames and save them to the SD card.

5. Once attached, the host will capture images cyclically and write them to the SD card. Following log shall be printed on the LOGUART console:
	```
	[UVC-I] USBH UVC demo start
	[MAIN-I] KM4 START SCHEDULER
	...
	[UVC-I] INIT
	[USBH-A] Device connect
	[USBH-A] Device attached,speed 0
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
	[UVC-I] Start vfs service
	...
	[UVC-I] VFS-SDcard Init Success
	[UVC-I] Stream on
	[UVC-I] Captured frame 0, len=25952
	[UVC-I] Create image file: sdcard:img0.jpeg
	[UVC-I] fwrite() ok, w 25946
	[UVC-I] Captured frame 1, len=24904
	[UVC-I] Create image file: sdcard:img1.jpeg
	...

6. Read the data in the SD card, and verify that the JPEG files are received and saved correctly.

For example, configure `USBH_UVC_HTTPC` and `USBH_UVC_FORMAT_MJPEG`:

Server Setup (Test PC Side)

1. Network Connection: Connect the Windows PC to the same Router/AP that the Ameba board will connect to.

2. Prepare Apache: Download https://projects.apache.org/releases.html and uzip apache24.7z to a local directory.

3. Configure `Apache24/conf/httpd.conf`:
   - Locate `SRVROOT` and modify the path to the directory where unzipped Apache.
   - Locate `Listen` and change it to PC's current IP address followed by port 5090.

4. Prepare `Apache24/cgi-bin/submit.py` (This script is necessary to enable file upload functionality on the Apache server.)

# Note

Verify that the path to python.exe corresponds to Test PC's Python installation directory.

Apache24/cgi-bin/submit.py:
```
#!C:\Programs\Python\Python312\python.exe
#!/usr/bin/python3
# -*- coding: utf-8 -*-

import cgi, os
import cgitb

cgitb.enable()

form = cgi.FieldStorage()

# get filename
fileitem = form['filename']

# check
if fileitem.filename:
    fn = os.path.basename(fileitem.filename)
    open('../uploads/' + fn, 'wb').write(fileitem.file.read())

    message = 'file "' + fn + '" upload success'

else:
    message = 'file upload fail'

print("Content-Type: text/html")
print()
print("""
<html>
<head>
    <meta charset="utf-8">
<title>rtk.upload.com</title>
</head>
<body>
   <p>%s</p>
</body>
</html>
""" % message)
```

5. Start Server:
   - Open a command prompt (CMD) inside the apache24/bin directory.
   - Run `httpd.exe`. The PC is now listening for incoming data from the board.

Board Operation

1. Configure the server IP `USBH_UVC_HTTPC_SERVER` to Test PC's current IP address.

2. Attach an USB camera to the USB port of the Ameba board.

3. Reset the board. Ensure the WiFi connection is established. Following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
	```
	[UVC-I] USBH UVC demo start
	```

4. Ameba board will recognize the USB camera. Once the video stream starts, it will capture frames and upload them to the configured HTTP server.

5. Once attached, the host will capture images cyclically and write them to the SD card. Following log shall be printed on the LOGUART console:
	```
	[UVC-I] USBH UVC demo start
	[MAIN-I] KM4 START SCHEDULER
	...
	[UVC-I] INIT
	[USBH-A] Device connect
	[USBH-A] Device attached,speed 0
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
	[UVC-I] Start HTTP client
	[UVC-I] Try to conn
	[UVC-I] Start httpc
	[UVC-I] Stream on
	[UVC-I] Captured frame 0, len=20448
	[UVC-I] Captured frame 1, len=19000
	[UVC-I] Stream on
	[UVC-I] Captured frame 0, len=20448
	[UVC-I] Captured frame 1, len=19000
	[UVC-I] Send image0 request ok
	[UVC-I] Captured frame 2, len=19008
	[UVC-I] Captured frame 3, len=24104
	[UVC-I] Captured frame 4, len=24056
	[UVC-I] Captured frame 5, len=71792
	[UVC-I] Captured frame 6, len=72032
	[UVC-I] Captured frame 7, len=71480
	[UVC-I] Captured frame 8, len=71536
	[UVC-I] Captured frame 9, len=71376
	[UVC-I] Send image1 request ok
	[UVC-I] Captured frame 10, len=72016
	[UVC-I] Captured frame 11, len=71672
	[UVC-I] Captured frame 12, len=71488
	[UVC-I] Captured frame 13, len=71760
	[UVC-I] Send image2 request ok
	...

6. Check the HTTP server side `Apache24/uploads` and verify that the JPEG files are received and saved correctly.

# Note

None

# Supported IC

RTL8730E
RTL8721F
