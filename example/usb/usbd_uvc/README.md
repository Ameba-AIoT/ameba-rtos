# Example Description

In this example, the Ameba board operates as a USB UVC (USB Video Class) device.

When connected to a USB host (PC), the host recognizes the Ameba board as a UVC device. You can then use PotPlayer to view the video stream from the board.

# Software Configuration

## 1. Configure the Project

Run `menuconfig` and enable the following options:

```text
[*] Enable USB
        USB Mode (Device)  --->
[*]     UVC
```

Save the configuration and exit.

## 2. Build and Download

- Refer to the **SDK Examples** section in the online documentation to build the project images.
- Download the generated images to the board using the **Ameba Image Tool**.

# Expected Result

1. Reset the board. The following messages should be displayed on the LOGUART console. The test PASSes only if the lines below show up and no error level log (`-E`) is reported.

   ```text
   [USBD-A] INIT
   [UVC-I] Initialization of extension unit handle
   [UVC-I] Get frame handler started
   [UVC-I] USBD UVC demo start
   ```

   Notes:
   * `[USBD-A] INIT` comes from the USB device core, and the two `[UVC-I]` lines come from the UVC class driver, not from the example.
   * On AmebaGreen2 an extra `[USB-I] UPHY para from ...` line is printed by the USB HAL before `[USBD-A] INIT`.

2. Connect the USB port of the Ameba board to a USB host (PC) using a USB cable.

3. The USB host should detect the Ameba board as a UVC device. The enumeration prints the descriptor and interface trace of the class driver, for example:

   ```text
   [UVC-I] Get descriptor USB_DESC_TYPE_DEVICE
   [UVC-I] Interface 0 alt 0
   [UVC-I] Interface 1 alt 0
   ```

4. Open **PotPlayer**, select the **USB UVC device** as the video source, choose the **H.264** video format, and verify that the video stream is displayed correctly. The host negotiates the format first, which prints the supported frame intervals and the committed format, then the stream starts:

   ```text
   [UVC-I] FPS 333333
   [UVC-I] FPS 666666
   [UVC-I] FPS 1000000
   [UVC-I] probe
   [UVC-I] Format = <fourcc> w = 1280 h = 720 fps = 30
   [UVC-I] commit
   [UVC-I] Interface 1 alt 1
   [UVC-I] Start to send the frame
   ```

   `[UVC-I] Start to send the frame` is the criterion that the stream has started. The negotiation lines above are emitted once per control request, so the host may repeat them several times before the stream settles. Closing the stream prints:

   ```text
   [UVC-I] Output queue empty
   [UVC-I] Input queue empty
   [UVC-I] Interface 1 alt 0
   ```

# Note

This example does not re-init the USB stack on cable detach, so no hotplug log sequence is printed. Re-plugging the cable makes the host enumerate and negotiate again, which repeats the descriptor, format and `Start to send the frame` lines of steps 3 and 4.

# Supported IC

RTL8730E
RTL8721F
RTL8735C