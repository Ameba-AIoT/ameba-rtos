# Example Description

In this application, Ameba acts as a USB UVC host to capture video frames from a USB camera. The example provides three configurable scenarios: discarding frames for testing, saving frames to a filesystem (e.g., SD card), or uploading frames to an HTTP server.

# HW Configuration

Some Ameba boards require hardware rework to support USB Host mode.
Populate the designated resistor footprint with a **0Ohm resistor** (or apply a solder bridge)
to enable VBUS power supply to the USB device.
Refer to the [EVB User Guide](https://aiot.realmcu.com/filelist?document_type=9) for details.

In addition, if testing the VFS SD card scenario, please adjust the SDIOH pinmux in usrcfg according to the hardware design.

## Board Rework Reference

### RTL8730E

| Board Variant      | Resistor | Rework Action                                  |
|--------------------|----------|------------------------------------------------|
| RTL8730EA / QFN100 | R20      | Populate with 0Ohm resistor or solder bridge     |
| RTL8730EL / QFN144 | R166     | Populate with 0Ohm resistor, or use an OTG cable |

### RTL8721F

| Board Variant    | Resistor | Rework Action                              |
|------------------|----------|--------------------------------------------|
| RTL8721FC0-R00   | R52      | Populate with 0Ohm resistor                 |
| RTL8721FA0-3V0   | R52      | Populate with 0Ohm resistor                 |
| RTL8721FL0-2V0   | R2       | Populate with 0Ohm resistor *(pre-reworked by default)* |

# SW Configuration

1. User Configurations

   Edit the macros below in `example_usbh_uvc.c` before building:

   | Macro                                    | Default                | Description                                                                                           |
   |------------------------------------------|------------------------|------------------------------------------------------------------------------------------------------ |
   | `CONFIG_USBH_UVC_APP`                    | `USBH_UVC_APP_SIMPLE`  | Supported application example: `USBH_UVC_APP_SIMPLE`, `USBH_UVC_APP_VFS`, `USBH_UVC_APP_HTTPC`        |
   | `CONFIG_USBH_UVC_FORMAT_TYPE`            | `USBH_UVC_FORMAT_MJPEG`| Supported formats: `USBH_UVC_FORMAT_MJPEG`, `USBH_UVC_FORMAT_YUV`, `USBH_UVC_FORMAT_H264`             |
   | `CONFIG_USBH_UVC_WIDTH`                  | `1280`                 | Target resolution and compression ratio (host stack selects closest match if camera does not support) |
   | `CONFIG_USBH_UVC_HEIGHT`                 | `720`                  | Target resolution and compression ratio (host stack selects closest match if camera does not support) |
   | `CONFIG_USBH_UVC_FRAME_RATE`             | `30`                   | Target resolution and compression ratio (host stack selects closest match if camera does not support) |
   | `CONFIG_USBH_UVC_FRAME_BUF_SIZE`         | `(150 * 1024)`         | Frame buffer size in bytes (increase if an oversize error occurs, see detailed calculation guide in example)                                     |
   | `CONFIG_USBH_UVC_IF_NUM_0`               | `0`                    | Video stream interface index (0 for single stream, 1 for dual streams)                                |
   | `CONFIG_USBH_UVC_HOT_PLUG`               | `1`                    | Hot plug / memory leak test                                                                           |
   | `CONFIG_USBH_UVC_CHECK_MJEPG_DATA`       | `1`                    | Check image data validity (0: Disable, 1: Enable)                                                     |
   | `CONFIG_USBH_UVC_LOOP`                   | `200`                  | Number of frames to capture in the loop                                                               |
   | `CONFIG_USBH_UVC_MAX_FAIL_COUNT`         | `5`                    | Maximum continuous error count before stopping                                                        |
   | `CONFIG_USBH_UVC_MAIN_THREAD_PRIORITY`   | `3`                    | Priority of the main UVC thread                                                                       |
   | `CONFIG_USBH_UVC_TEST_THREAD_PRIORITY`   | `4`                    | Priority of the UVC test thread                                                                       |
   | `CONFIG_USBH_UVC_HOTPLUG_THREAD_PRIORITY`| `6`                    | Priority of the USB hotplug detection thread                                                          |
   | `CONFIG_USBH_UVC_HW_IRQ_PRIORITY`        | `INT_PRI_LOWEST`       | HW UVC IRQ PRIORITY                                                                                   |

   > **Note:** The host stack automatically selects the closest supported resolution/format/rate if the camera does not support the configured values. Always check the log output to confirm the actual parameters applied.

2. Menuconfig

   Run menuconfig and select `CONFIG USB`:
   ```
   [*] Enable USB
           USB Mode (Host)  --->
   [*] UVC
   ```

   If configuring `USBH_UVC_APP_VFS`, additionally run `./menuconfig.py` under the project directory and select `CONFIG VFS`:
   ```
   [*] Enable VFS FATFS
           FATFS Memory Type (SD MODE)  --->
   ```

   Save and exit.

3. Build and Download
   - Refer to the SDK Examples section of the online documentation to generate images.
   - Download images to the board using the Ameba Image Tool.

# Expect Result

## Scenario: SIMPLE (Default)

Attach a USB camera to the USB port of the Ameba development board, then reset the board. The LOGUART console shall print the following output. Confirm that no USB-related errors are reported.

```
[UVC-I] USBH UVC demo start                    #criteria: init
[MAIN-I] KM4 START SCHEDULER
...
[UVC-I] INIT                                   #criteria: init_complete
[UVC-I] ATTACH                                 #criteria: attach
[UVC-I] SETUP                                  #criteria: setup
[UVC-I] SETPARAM                               #criteria: set_param
[UVC-I] Stream on                              #criteria: stream_on
[UVC-I] Captured frame 0, len=20832
...
[UVC-I] Captured frame 199, len=108264         #criteria: capture_complete
[UVC-I] TP 4126 KB/s @ 4953 ms                 #criteria: throughput
[UVC-I] Stream off                             #criteria: stream_off
```

> **Note:** Values shown above (frame lengths, throughput, timestamps) are examples. Actual values vary depending on the camera and connection. The `Captured frame 199` line confirms all 200 frames (0-199) were captured successfully.

## Scenario: VFS

1. Insert a FAT32-formatted SD card into the SD card slot of the Ameba development board.
2. Attach a USB camera to the USB port of the Ameba development board.
3. Reset the board. The LOGUART console shall print the following output. Confirm that no USB-related errors are reported.

```
[UVC-I] USBH UVC demo start                    #criteria: init
[UVC-I] INIT
...
[UVC-I] SETUP
[UVC-I] Stream on
[UVC-I] VFS-SDcard Init Success                #criteria: vfs_ready
[UVC-I] Create image file: sdcard:img0.jpeg    #criteria: vfs_write
[UVC-I] fwrite() ok, w 25946                   #criteria: vfs_write_ok
...
```

4. Read the data from the SD card and verify that the JPEG files were received and saved correctly.

## Scenario: HTTPC

### Server Setup (Test PC Side)

1. Connect the Windows PC to the same router or access point that the Ameba development board will use.
2. Download Apache from https://projects.apache.org/releases.html and extract `apache24.7z` to a local directory.
3. Edit `Apache24/conf/httpd.conf`:
   - Locate `SRVROOT` and update the path to the directory where Apache was extracted.
   - Locate `Listen` and change it to the PC's current IP address followed by port 5090.
4. Create `Apache24/cgi-bin/submit.py`:

   Verify that the path to python.exe corresponds to the Test PC's Python installation directory.

   ```python
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

5. Start the server:
   - Open a command prompt (CMD) inside the `apache24/bin` directory.
   - Run `httpd.exe`. The PC is now listening for incoming data from the board.

### Board Operation

1. Configure the server IP `USBH_UVC_HTTPC_SERVER` to the Test PC's current IP address in `example_usbh_uvc.c`.
2. Attach a USB camera to the USB port of the Ameba development board.
3. Reset the board. Ensure that the WiFi connection is established. The LOGUART console shall print the following output. Confirm that no USB-related errors are reported.

```
[UVC-I] USBH UVC demo start                    #criteria: init
...
[UVC-I] SETUP
[UVC-I] Start httpc                            #criteria: httpc_start
[UVC-I] Stream on
[UVC-I] Send image0 request ok                 #criteria: httpc_upload_ok
...
```

4. Check the HTTP server side `Apache24/uploads` directory and verify that the JPEG files were received and saved correctly.

# Note

None

# Supported IC

RTL8730E
RTL8721F
