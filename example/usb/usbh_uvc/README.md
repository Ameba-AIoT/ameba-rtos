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
   | `CONFIG_USBH_UVC_FRAME_BUF_SIZE`         | see source             | Frame buffer size in bytes (increase if a truncation error occurs; default defined in `example_usbh_uvc.c`, may change per resolution/format)     |
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
```

> **Note:** Values shown above (frame lengths, throughput, timestamps) are examples. Actual values vary depending on the camera and connection. The `Captured frame 199` line confirms all 200 frames (0-199) were captured successfully.
>
> **Note:** If a `truncated` error appears in the log, the camera frame was larger than the buffer and its tail was dropped. Check whether the `len` in the nearby `Captured frame` line has reached `CONFIG_USBH_UVC_FRAME_BUF_SIZE`. If so, increase `CONFIG_USBH_UVC_FRAME_BUF_SIZE` in `example/usb/usbh_uvc/example_usbh_uvc.c`.

## Scenario: VFS

1. Modify the `CONFIG_USBH_UVC_APP` in `example\usb\usbh_uvc\example_usbh_uvc.c` to `USBH_UVC_APP_VFS`.
2. Ensure that, according to the `SW Configuration` instructions described earlier, support for `VFS -> FATFS Memory Type (SD MODE)` has been enabled via `menuconfig`.
3. Rebuild and Download.
4. Insert a FAT32-formatted SD card into the SD card slot of the Ameba development board.
5. Attach a USB camera to the USB port of the Ameba development board.
6. Reset the board. The LOGUART console shall print the following output. Confirm that no USB-related errors are reported. The saved file layout depends on `CONFIG_USBH_UVC_FORMAT_TYPE`:

   **MJPEG** — each captured frame is written to its own `imgN.jpeg` file. Capture stops after `CONFIG_USBH_UVC_LOOP` frames.

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
[UVC-I] VFS MJPEG done, 195 files written to SD #criteria: vfs_done
```

   **H264 / H265** — the whole stream is written to a single `stream.h264` / `stream.h265` file. Capture stops once the file reaches the `USBH_UVC_VFS_VIDEO_SIZE` (2 MB) target, not after a fixed frame count.

```
[UVC-I] USBH UVC demo start                    #criteria: init
[UVC-I] INIT
...
[UVC-I] SETUP
[UVC-I] Stream on
[UVC-I] VFS-SDcard Init Success                #criteria: vfs_ready
[UVC-I] Create image file: sdcard:stream.h264  #criteria: vfs_write
...
[UVC-I] VFS stream.h264 done, 2097152 bytes    #criteria: vfs_done
```

4. Read the data from the SD card and verify that the received files were saved correctly: the `imgN.jpeg` files for MJPEG, or the single `stream.h264` / `stream.h265` file for H264 / H265.

## Scenario: HTTPC

### Server Setup (Test PC Side)

1. Modify the `CONFIG_USBH_UVC_APP` in `example\usb\usbh_uvc\example_usbh_uvc.c` to `USBH_UVC_APP_HTTPC`.
2. Rebuild and Download
3. Connect the Windows PC to the same router that the Ameba development board will use. The topology:
```
[Camera] ---USB Cable--- [Ameba] ---Wifi--- [Router] --- [PC]
```
4. Download Apache from https://projects.apache.org/releases.html and extract `apache24.7z` to a local directory.
5. Edit `Apache24/conf/httpd.conf`:
   - Locate `SRVROOT` and update the path to the directory where Apache was extracted.
   - Locate `Listen` and change it to the PC's current IP address followed by port 5090.
6. Create `Apache24/cgi-bin/submit.py`:

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

7. Start the server:
   - Open a command prompt (CMD) inside the `apache24/bin` directory.
   - Run `httpd.exe`. The PC is now listening for incoming data from the board.
   > **Note:** `httpd.exe` may exit on startup instead of staying resident. Verify it is listening before testing the board — in a separate CMD window run `netstat -an | findstr 5090` and confirm a `x.x.x.x:5090 ... LISTENING` line. If nothing is listening, check `Apache24\logs\error.log` for the cause, or run `httpd.exe -X` to print the startup error directly. For example, `vcruntime140.dll ... is not compatible` means the Visual C++ Redistributable (x64, VS 2015–2022) must be updated — download it from https://aka.ms/vs/17/release/vc_redist.x64.exe .

### Board Operation

1. Configure the HTTP server IP so the board knows where to upload frames:
   - **Compile-time:** set `USBH_UVC_HTTPC_SERVER` in `example_usbh_uvc.c` to the Test PC's IP, then rebuild an download;
   - **Runtime (no rebuild):** after reset, enter the following LOGUART command to override the server IP.
   ```
   uvch_setip <ip>
   ```
   On success the console prints `HTTPC server IP set to: <ip>`. Enter this command before the WiFi/HTTPC connection is established.
2. Rebuild and Download.
3. Attach a USB camera to the USB port of the Ameba development board.
4. Reset the board. Connect to the router by entering the following AT command, Verify that the WiFi connection is successfully established before proceeding.
   ```
   AT+WLCONN=ssid,<SSID>,pw,<PASSWORD>
   ```

   > **Note:** Replace <SSID> with your Wi-Fi network name and <PASSWORD> with the corresponding Wi-Fi password.

5. The LOGUART console shall print the following output. Confirm that no USB-related errors are reported. The upload behavior depends on `CONFIG_USBH_UVC_FORMAT_TYPE`:

   **MJPEG** — each captured frame is POSTed as one HTTP request. Upload stops after `CONFIG_USBH_UVC_LOOP` frames.

```
[UVC-I] USBH UVC demo start                    #criteria: init
...
[UVC-I] SETUP
[UVC-I] Start httpc                            #criteria: httpc_start
[UVC-I] Stream on
[UVC-I] Send image0 request ok                 #criteria: httpc_upload_ok
...
```

   **H264 / H265** — the stream is uploaded in bulk rather than per-frame. When hardware UVC is enabled, frames are first accumulated in PSRAM and then POSTed together; otherwise the stream is POSTed continuously until the 2 MB `USBH_UVC_HTTPC_VIDEO_SIZE` target is reached. Upload is not bounded by a fixed frame count.

```
[UVC-I] USBH UVC demo start                    #criteria: init
...
[UVC-I] SETUP
[UVC-I] Start httpc                            #criteria: httpc_start
[UVC-I] Stream on
[UVC-I] HTTP: 60 frames (160951 bytes) ready for upload
[UVC-I] HTTP: PSRAM has 160951 bytes from 60 frames
[UVC-I] HTTP: Upload complete: 160951 bytes (60 frames) #criteria: httpc_upload_ok
...
```

6. Check the HTTP server side `Apache24/uploads` directory and verify that the received files were saved correctly: the per-frame JPEG files for MJPEG, or the `stream.h264` / `stream.h265` file for H264 / H265.

# Note

None

# Supported IC

RTL8730E
RTL8721F
