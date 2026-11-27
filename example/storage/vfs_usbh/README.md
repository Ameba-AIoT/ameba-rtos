# Example Description

This is an example of using virtual file system on a USB mass-storage device attached to the USB host port.

# HW Configuration

Attach a USB storage device (FAT/exFAT formatted) to the USB host port of the board.

# SW configuration

1. Fixed Parameters for USB Registration:
   The USB mass-storage device (FATFS) is registered with:
   `vfs_target_register("usbh", VFS_TARGET_USBH, VFS_RW);`
   the argument `VFS_TARGET_USBH` must remain unchanged.

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

1. Write vfs file (file_name = "vfs_file_1", content = "VFS usbh module works normally !!!") succeeded

2. Read vfs file succeeded

3. Delete vfs file succeeded

# Note

The external FATFS targets (SD card, USB host, ...) share one FATFS instance, only one target can be mounted at a time, mounting a different one fails with `target <n> busy`. Registering the same target again under another prefix succeeds as an alias.

On `RTL8735C` the SD card is registered at boot. If it is mounted successfully (an SD card is inserted), add the following to `prj.conf`, otherwise the registration fails with `target 7 busy`:
```
CONFIG_VFS_SD_CARD=n
```

# Supported IC

RTL8730E
RTL8721F
RTL8735C
