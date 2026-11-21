# Example Description

In this example, the Ameba board operates as a USB UVC (USB Video Class) device.

When connected to a USB host (PC), the host recognizes the Ameba board as a UVC device. You can then use PotPlayer (Windows) or ffplay / guvcview (Linux) to view the video stream from the board.

The device declares YUY2, NV12, MJPEG, H.264 and H.265 formats at 1920x1080 and 30/15/10 fps, but the example always streams the built-in H.264 sample clip (`example_usbd_uvc_sample_h264.h`) at 30 fps whatever format is committed, so only the H.264 format carries valid data.

# Software Configuration

1. Default configuration

   The `prj.conf` of this example sets the options below by default, no menuconfig is needed to run it:
   ```text
   CONFIG USB --->
   [*] Enable USB
           USB Mode (Device)  --->
   [*]     UVC
   ```

2. Build

   ```
   ./ameba.py build -a usbd_uvc -p
   ```
   `-p` (pristine) drops the previous configuration and applies the `prj.conf` of this example.

3. (Optional) Change the configuration

   Only needed when the default configuration does not fit:
   1. Build once with `-p` as in step 2, so that the configuration starts from `prj.conf`.
   2. Run `./ameba.py menuconfig` and change the options.
   3. Rebuild without `-p`: `./ameba.py build -a usbd_uvc`.

   To keep a change permanently, add its `CONFIG_XXX=y` lines to `prj.conf` instead and build with `-p`.

4. Download

   Flash the images to the board with any of the tools below:
   * VS Code extension: https://aiot.realmcu.com/en/latest/tools/vscode/index.html
   * Image Tool (Windows): https://aiot.realmcu.com/en/latest/tools/image_tool/index.html
   * Flash Tool (cross-platform): https://aiot.realmcu.com/en/latest/tools/flash/index.html
   * Web flash tool: https://aiot.realmcu.com/en/resource/web_flash_tool.html

# Expected Result

1. Reset the board. The following messages should be displayed on the LOGUART console. The test PASSes only if the lines below show up and no error level log (`-E`) is reported.

   ```text
   [UVC-I] USBD UVC demo start
   [USBD-A] INIT
   [UVC-I] Initialization of extension unit handle
   [UVC-I] Get frame handler started
   ```

   Notes:
   * `[USBD-A] INIT` comes from the USB device core, and the two `[UVC-I]` lines come from the UVC class driver, not from the example.
   * On AmebaGreen2 (RTL8721F) an extra USB PHY calibration line is printed by the USB HAL right after `[USBD-A] INIT`: `[USB-I] UPHY para from autoload`, `[USB-I] UPHY para from OTP` or `[USB-I] UPHY OTP flag 0x<flag>, use default`.
   * Failure lines: `[UVC-E] Create USBD UVC thread fail`, `[UVC-E] Init USBD fail`, `[UVC-E] Init UVC class fail` or `[UVC-E] Create USBD UVC video thread fail`, followed by `[UVC-E] USBD UVC demo aborted` and `[UVC-E] Start fail`.

2. Connect the USB port of the Ameba board to a USB host (PC) using a USB cable.

3. The USB host should detect the Ameba board as a UVC device. The enumeration prints the descriptor and interface trace of the class driver, for example:

   ```text
   [UVC-I] Get descriptor USB_DESC_TYPE_DEVICE
   [UVC-I] Interface 0 alt 0
   [UVC-I] Interface 1 alt 0
   ```

4. Open **PotPlayer**, select the **USB UVC device** as the video source, choose the **H.264** video format, and verify that the video stream is displayed correctly. The host negotiates the format first, which prints the supported frame intervals (in 100 ns units) and the committed format, then the stream starts, for example:

   ```text
   [UVC-I] FPS 333333
   [UVC-I] FPS 666666
   [UVC-I] FPS 1000000
   [UVC-I] Type 3
   [UVC-I] probe
   [UVC-I] Format = 3 w = 1920 h = 1080 fps = 333333
   [UVC-I] commit
   [UVC-I] Start to send the frame
   [UVC-I] Interface 1 alt 1
   [UVC-I] Key frame reached, streaming starts
   ```

   `Format = <n>` prints the format type (0 YUY2, 1 NV12, 2 MJPEG, 3 H.264, 4 H.265) and `fps = <n>` the committed frame interval in 100 ns units (333333 = 30 fps). `[UVC-I] Start to send the frame` followed by `[UVC-I] Key frame reached, streaming starts` is the criterion that the stream has started: payloads are held back until the first key frame of the clip, a `[UVC-W] No key frame within 90 frames, gate opened` line means none was found. The negotiation lines above are emitted once per control request, so the host may repeat them several times before the stream settles. Closing the stream prints:

   ```text
   [UVC-I] Output queue empty
   [UVC-I] Input queue empty
   [UVC-I] Interface 1 alt 0
   ```

5. Host side check:
   - Linux: list the formats, then play the H.264 stream (`/dev/videoN` is the node of `Realtek UVC Device`):

     ```text
     v4l2-ctl --list-devices
     v4l2-ctl -d /dev/videoN --list-formats-ext
     ffplay -f v4l2 -input_format h264 -video_size 1920x1080 -i /dev/videoN
     ```

     `guvcview -d /dev/videoN` also works once `H264` is selected as the format.
   - Windows: use PotPlayer as in step 4. The Windows Camera app chooses the format by itself and may pick a non-H.264 one, which shows no valid picture with this example; use a player where H.264 can be selected.

# Note

This example does not re-init the USB stack on cable detach, so no hotplug log sequence is printed. Re-plugging the cable makes the host enumerate and negotiate again, which repeats the descriptor, format and `Start to send the frame` lines of steps 3 and 4.

# Supported IC

RTL8730E
RTL8721F
RTL8735C