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

1. Reset the board. The following message should be displayed on the LOGUART console. Verify that no USB-related errors are reported.

   ```text
   [UVC-I] USBD UVC demo start
   ```

2. Connect the USB port of the Ameba board to a USB host (PC) using a USB cable.

3. The USB host should detect the Ameba board as a UVC device.

4. Open **PotPlayer**, select the **USB UVC device** as the video source, choose the **H.264** video format, and verify that the video stream is displayed correctly.

# Note

None.

# Supported IC

- RTL8735C