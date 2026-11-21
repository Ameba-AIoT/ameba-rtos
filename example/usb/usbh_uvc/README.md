# Example Description

In this application, Ameba acts as a USB UVC host to capture video frames from a USB camera. The example provides three configurable scenarios: discarding frames for testing, saving frames to a filesystem (e.g., SD card), or uploading frames to an HTTP server.

Camera requirements:
- USB Video Class camera (UVC 1.0/1.1/1.5) streaming over an **isochronous IN** endpoint; bulk-streaming cameras are not supported.
- One of the formats MJPEG, uncompressed YUV (YUY2), H264 or H265 (frame-based). YUV is only supported by the `SIMPLE` scenario.
- On SoCs with the UVC hardware decoder (RTL8721F) at most 2 video streams are supported.

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
   | `CONFIG_USBH_UVC_FORMAT_TYPE`            | `USBH_UVC_FORMAT_MJPEG`| Supported formats: `USBH_UVC_FORMAT_MJPEG`, `USBH_UVC_FORMAT_YUV`, `USBH_UVC_FORMAT_H264`, `USBH_UVC_FORMAT_H265`. `USBH_UVC_FORMAT_YUV` is only allowed with `USBH_UVC_APP_SIMPLE` (build error otherwise) |
   | `CONFIG_USBH_UVC_WIDTH`                  | `1280`                 | Target resolution and compression ratio (host stack selects closest match if camera does not support) |
   | `CONFIG_USBH_UVC_HEIGHT`                 | `720`                  | Target resolution and compression ratio (host stack selects closest match if camera does not support) |
   | `CONFIG_USBH_UVC_FRAME_RATE`             | `30`                   | Target resolution and compression ratio (host stack selects closest match if camera does not support) |
   | `CONFIG_USBH_UVC_FRAME_BUF_SIZE`         | `170 * 1024`           | Frame buffer size in bytes. A frame larger than this is dropped; increase it if a truncation warning occurs (sizing guide in `example_usbh_uvc.c`) |
   | `CONFIG_USBH_UVC_STREAM_INDEX`           | `0`                    | Video stream index (0 for single stream; 0 or 1 for a dual-stream camera, HW UVC supports up to 2 streams) |
   | `CONFIG_USBH_UVC_HOT_PLUG`               | `1`                    | Hot plug / memory leak test                                                                           |
   | `CONFIG_USBH_UVC_CHECK_MJEPG_DATA`       | `1` for MJPEG, else `0`| Check the JPEG SOI/EOI markers of every frame; derived from `CONFIG_USBH_UVC_FORMAT_TYPE`, only effective for MJPEG |
   | `CONFIG_USBH_UVC_LOOP`                   | `200`                  | Number of frames to capture in the loop (`SIMPLE`, and MJPEG in `VFS`/`HTTPC`)                       |
   | `CONFIG_USBH_UVC_MAX_FAIL_COUNT`         | `5`                    | Maximum continuous error count before stopping                                                        |
   | `CONFIG_USBH_UVC_MAIN_THREAD_PRIORITY`   | `3`                    | Priority of the main UVC thread                                                                       |
   | `CONFIG_USBH_UVC_TEST_THREAD_PRIORITY`   | `4`                    | Priority of the UVC test thread                                                                       |
   | `CONFIG_USBH_UVC_HOTPLUG_THREAD_PRIORITY`| `6`                    | Priority of the USB hotplug detection thread                                                          |
   | `CONFIG_USBH_UVC_HW_IRQ_PRIORITY`        | `INT_PRI_LOWEST`       | HW UVC IRQ priority, only defined on SoCs with the UVC hardware decoder (`USBH_UVC_USE_HW`, RTL8721F)  |

   > **Note:** The host stack automatically selects the closest supported resolution/format/rate if the camera does not support the configured values. Always check the log output to confirm the actual parameters applied.

2. Default configuration

   The `prj.conf` of this example sets the options below by default, no menuconfig is needed to run it:
   ```
   CONFIG USB --->
   [*] Enable USB
           USB Mode (Host)  --->
   [*] UVC
   ```

3. Build

   ```
   ./ameba.py build -a usbh_uvc -p
   ```
   `-p` (pristine) drops the previous configuration and applies the `prj.conf` of this example.

4. (Optional) Change the configuration

   Only needed when the default configuration does not fit:
   1. Build once with `-p` as in step 3, so that the configuration starts from `prj.conf`.
   2. Run `./ameba.py menuconfig` and change the options.
   3. Rebuild without `-p`: `./ameba.py build -a usbh_uvc`.

   To keep a change permanently, add its `CONFIG_XXX=y` lines to `prj.conf` instead and build with `-p`.

   If configuring `USBH_UVC_APP_VFS`, additionally select `CONFIG File System`:
   ```
   [*] Enable VFS
   [*]     SD card, SDIO mode
               File system on SD card (FATFS)  --->
   ```
   Or add the following lines to `prj.conf` of this example (parent symbol first), build with `-p` and restore `prj.conf` after the test:
   ```
   CONFIG_VFS_SD_CARD=y
   CONFIG_VFS_SD_FATFS=y
   ```
   Do not write the derived symbols (e.g. `CONFIG_FATFS_SD_MODE`, `CONFIG_VFS_FATFS_INCLUDED`) into `prj.conf`, they follow the choices above.

5. Download

   Flash the images to the board with any of the tools below:
   * VS Code extension: https://aiot.realmcu.com/en/latest/tools/vscode/index.html
   * Image Tool (Windows): https://aiot.realmcu.com/en/latest/tools/image_tool/index.html
   * Flash Tool (cross-platform): https://aiot.realmcu.com/en/latest/tools/flash/index.html
   * Web flash tool: https://aiot.realmcu.com/en/resource/web_flash_tool.html

# Expect Result

## Scenario: SIMPLE (Default)

Attach a USB camera to the USB port of the Ameba development board, then reset the board. The LOGUART console shall print the following output. Confirm that no USB-related errors are reported.

```
[UVC-I] USBH UVC demo start                    #criteria: init
...
[UVC-I] INIT                                   #criteria: init_complete
[UVC-I] ATTACH                                 #criteria: attach
[UVC-I] Set paras ok: MJPEG 1280*720@30fps     #criteria: set_param
[UVC-I] Captured frame 1, len=20832
...
[UVC-I] Captured frame 200, len=108264         #criteria: capture_complete
[UVC-I] TP 4126 KB/s @ 6653 ms, fps 30/s       #criteria: throughput
[UVC-I] TP 4.02 MB/s (28109312 bytes)
```

> **Note:** Values shown above (frame lengths, throughput, timestamps) are examples. Actual values vary depending on the camera and connection. The `Set paras ok: <fmt> <w>*<h>@<fps>fps` line shows the parameters actually applied. The `Captured frame 200` line confirms all 200 frames (1-200) were captured successfully. After the capture the demo waits for the next attach: unplug and re-plug the camera to run it again.
>
> **Note:** If the log shows `[UVC-W] Frm > buf <n>, truncated, enlarge frame_buf_size`, a camera frame was larger than `CONFIG_USBH_UVC_FRAME_BUF_SIZE` and that frame was dropped (it is never handed to the application). Increase `CONFIG_USBH_UVC_FRAME_BUF_SIZE` in `example/usb/usbh_uvc/example_usbh_uvc.c`. `[UVC-W] Dev frm size <n> > buf <n>, frames may be dropped` printed at stream setup is an early hint of the same problem.
>
> **Note:** With MJPEG, a frame without valid JPEG SOI/EOI markers is reported as `[UVC-E] [mjpeg] image error: ...` or `[UVC-E] [mjpeg] image too short: <n>`; capture continues. `[UVC-I] Stop (fail:5)` means no frame arrived for `CONFIG_USBH_UVC_MAX_FAIL_COUNT` consecutive tries. `Set paras err: ...` / `Set paras timeout: ...` means the camera rejected or did not answer the stream negotiation.

**Hotplug** (`CONFIG_USBH_UVC_HOT_PLUG` 1): unplug the camera, the stack is torn down and re-initialized:

```
[UVC-I] DETACH
[UVC-I] Test exit on detach
[UVC-I] Test thread exit
[UVC-I] DEINIT
[UVC-I] Free heap: 0x...
[UVC-I] INIT
[UVC-I] Reinit done
```

`Test exit on detach` is only printed when the camera is unplugged during a capture. Re-plug the camera and the capture runs again. The `Free heap` value shall stay stable across cycles. If the stack cannot be rebuilt, `[UVC-E] Reinit fail` and `[UVC-E] Hotplug thread exit` are printed.

## Scenario: VFS

1. Modify the `CONFIG_USBH_UVC_APP` in `example\usb\usbh_uvc\example_usbh_uvc.c` to `USBH_UVC_APP_VFS`.
2. Enable `SD card, SDIO mode` with `File system on SD card (FATFS)` as described in step 4 of `SW Configuration` (menuconfig or `prj.conf`).
3. Rebuild as above and Download.
4. Insert a FAT32-formatted SD card into the SD card slot of the Ameba development board.
5. Attach a USB camera to the USB port of the Ameba development board.
6. Reset the board. The LOGUART console shall print the following output. Confirm that no USB-related errors are reported. The files are written to the root directory of the SD card. The saved file layout depends on `CONFIG_USBH_UVC_FORMAT_TYPE`:

   **MJPEG** — each captured frame is written to its own `imgN.jpeg` file, numbered from `img0.jpeg`. Capture stops after `CONFIG_USBH_UVC_LOOP` frames; a frame that arrives while the previous one is still being written is skipped, so the file count may be smaller than the frame count.

```
[UVC-I] USBH UVC demo start                    #criteria: init
[UVC-I] INIT
...
[UVC-I] ATTACH
[UVC-I] Start vfs service
[UVC-I] VFS-SDcard init done                   #criteria: vfs_ready
[UVC-I] Set paras ok: MJPEG 1280*720@30fps
[UVC-I] Captured frame 1, len=25946
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
[UVC-I] ATTACH
[UVC-I] Start vfs service
[UVC-I] VFS-SDcard init done                   #criteria: vfs_ready
[UVC-I] Create image file: sdcard:stream.h264  #criteria: vfs_write
[UVC-I] Set paras ok: H264 1280*720@30fps
...
[UVC-I] VFS stream.h264 done, 2097152 bytes    #criteria: vfs_done
```

   The order of the `VFS-SDcard init done` / `Create image file` and `Set paras ok` lines may vary for H264 / H265.

   Failure logs: `VFS-SDcard init fail` (no or unformatted SD card, wrong SDIOH pinmux), `fopen image fail`, `fwrite() failed, err: <n>`, `buf fwrite fail: <n>`, `VFS buf alloc fail`.

7. Read the data from the SD card and verify that the received files were saved correctly: the `imgN.jpeg` files for MJPEG, or the single `stream.h264` / `stream.h265` file for H264 / H265.

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
   - The board POSTs to `http://<PC IP>:5090/cgi-bin/submit.py` (multipart/form-data, form field `filename`); the default server IP in the firmware is `192.168.1.102`, see `Board Operation` below to change it.
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

   Create the upload directory `Apache24/uploads` (the script writes to `../uploads/` relative to `cgi-bin`, and does not create it).

7. Start the server:
   - Open a command prompt (CMD) inside the `apache24/bin` directory.
   - Run `httpd.exe`. The PC is now listening for incoming data from the board.
   > **Note:** `httpd.exe` may exit on startup instead of staying resident. Verify it is listening before testing the board — in a separate CMD window run `netstat -an | findstr 5090` and confirm a `x.x.x.x:5090 ... LISTENING` line. If nothing is listening, check `Apache24\logs\error.log` for the cause, or run `httpd.exe -X` to print the startup error directly. For example, `vcruntime140.dll ... is not compatible` means the Visual C++ Redistributable (x64, VS 2015–2022) must be updated — download it from https://aka.ms/vs/17/release/vc_redist.x64.exe .

### Board Operation

1. Configure the HTTP server IP so the board knows where to upload frames:
   - **Compile-time:** set `USBH_UVC_HTTPC_SERVER` (default `192.168.1.102`) in `example_usbh_uvc.c` to the Test PC's IP, then rebuild and download;
   - **Runtime (no rebuild):** after reset, enter the following LOGUART command to override the server IP.
   ```
   AT+USBHUVC=ip,<a.b.c.d>
   ```
   `<a.b.c.d>` is a dotted IPv4 address, each field 0..255; anything else is rejected. On success the console prints `HTTPC server IP set to: <ip>`. Enter this command before the WiFi/HTTPC connection is established. `AT+USBHUVC=help` (or `AT+USBHUVC=?`) prints the usage and the default IP. The command only exists on an HTTPC build.
2. Attach a USB camera to the USB port of the Ameba development board.
3. Reset the board. Connect to the router by entering the following AT command, Verify that the WiFi connection is successfully established before proceeding. The capture only starts after WiFi is connected.
   ```
   AT+WLCONN=ssid,<SSID>,pw,<PASSWORD>
   ```

   > **Note:** Replace <SSID> with your Wi-Fi network name and <PASSWORD> with the corresponding Wi-Fi password.

4. The LOGUART console shall print the following output. Confirm that no USB-related errors are reported. The upload behavior depends on `CONFIG_USBH_UVC_FORMAT_TYPE`:

   **MJPEG** — each captured frame is POSTed as one HTTP request with file name `img<N>.jpeg`, numbered from `img0.jpeg`. Upload stops after `CONFIG_USBH_UVC_LOOP` frames. `Try to conn` repeats until the server is reachable; a failed frame upload is retried once on a new connection (`Send HTTP request fail: <err> reconn=<n>`).

```
[UVC-I] USBH UVC demo start                    #criteria: init
...
[UVC-I] ATTACH
[UVC-I] Start HTTP client (ringbuffer mode)
[UVC-I] Try to conn
[UVC-I] Start httpc                            #criteria: httpc_start
[UVC-I] Set paras ok: MJPEG 1280*720@30fps
[UVC-I] Captured frame 1, len=25946
[UVC-I] Send image0 request ok                 #criteria: httpc_upload_ok
...
```

   **H264 / H265** — the stream is uploaded in bulk as one file `stream.h264` / `stream.h265`, not per-frame. Two modes are selected at compile time:

   - **Ringbuffer mode** (SoCs without the UVC hardware decoder, e.g. RTL8730E): the stream is POSTed continuously until the 2 MB `USBH_UVC_HTTPC_VIDEO_SIZE` target is reached. `Try to connect http server` repeats until the server is reachable.

```
[UVC-I] USBH UVC demo start                    #criteria: init
...
[UVC-I] ATTACH
[UVC-I] Start HTTP client (ringbuffer mode)
[UVC-I] Try to connect http server
[UVC-I] Start httpc                            #criteria: httpc_start
[UVC-I] Set paras ok: H264 1280*720@30fps
...
[UVC-I] HTTP: Upload complete: 2097152 bytes   #criteria: httpc_upload_ok
```

   - **PSRAM mode** (SoCs with the UVC hardware decoder, RTL8721F): frames are first captured into a 2 MB buffer allocated from the PSRAM heap for about 2 s (`2 * CONFIG_USBH_UVC_FRAME_RATE` frames) or until the buffer is full (`HTTP: PSRAM full (...), stop capture`), then POSTed together. The connect is tried up to 10 times (`USBH_UVC_HTTPC_CONNECT_RETRY`, 10 s timeout each), then the upload is aborted with `HTTP: connect failed after 10 attempts, upload aborted`. The upload is skipped if the chip has no PSRAM (`HTTP: no PSRAM, upload skipped`) or the buffer cannot be allocated (`HTTP: PSRAM buffer alloc fail, upload skipped`); on RLE1509 add `CONFIG_DISABLE_PSRAM=n` to `prj.conf`, as PSRAM is disabled by default there.

```
[UVC-I] USBH UVC demo start                    #criteria: init
...
[UVC-I] ATTACH
[UVC-I] Start HTTP client (PSRAM buffer mode)
[UVC-I] Set paras ok: H264 1280*720@30fps
...
[UVC-I] HTTP: 60 frames (160951 bytes) ready for upload
[UVC-I] HTTP: PSRAM has 160951 bytes from 60 frames
[UVC-I] Try to connect HTTP server
[UVC-I] Start HTTP                             #criteria: httpc_start
[UVC-I] HTTP: Upload complete: 160951 bytes (60 frames) #criteria: httpc_upload_ok
```

5. Check the HTTP server side `Apache24/uploads` directory and verify that the received files were saved correctly: the `img<N>.jpeg` files for MJPEG, or the `stream.h264` / `stream.h265` file for H264 / H265.

# Note

None

# Supported IC

RTL8730E
RTL8721F
