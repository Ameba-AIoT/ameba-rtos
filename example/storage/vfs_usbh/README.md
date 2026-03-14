# Example Description

This is an example of using virtual file system on usb device.

# HW Configuration

None

# SW configuration

# SW configuration
1. Fixed Parameters for USB Registration:
   When registering FATFS on usb device using:
   `vfs_user_register("usbh", VFS_FATFS, VFS_INF_USBH, VFS_REGION_4, VFS_RW);`
   the arguments `VFS_FATFS`, `VFS_INF_USBH` and `VFS_REGION_4` must remain unchanged.

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
