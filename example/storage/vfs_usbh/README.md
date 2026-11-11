# Example Description

This is an example of using virtual file system on usb device.

# HW Configuration

None

# SW configuration

# SW configuration
1. Fixed Parameters for USB Registration:
   When registering FATFS on usb device using:
   `vfs_target_register("usbh", VFS_TARGET_USBH, VFS_RW);`
   the argument `VFS_TARGET_USBH` must remain unchanged. The file system (FATFS) is decided by Kconfig (`CONFIG_VFS_USB_HOST`), not by the caller.

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

1. Write vfs file (file_name = "vfs_file_1", content = "VFS usbh module works normally !!!") succeeded

2. Read vfs file succeeded

3. Delete vfs file succeeded

# Note

None

# Supported IC

RTL8730E
RTL8721F
RTL8735C
