# Example Description

This is an example of using virtual file system on SD card.

# HW Configuration

None

# SW configuration

# SW configuration
1. Fixed Parameters for SD Card Registration:
   When registering FATFS on SD Card using:
   `vfs_target_register("sdcard", VFS_TARGET_SD, VFS_RW);`
   the argument `VFS_TARGET_SD` must remain unchanged. The file system (FATFS) is decided by Kconfig (`CONFIG_VFS_SD_FATFS`), not by the caller.

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

1. Write vfs file (file_name = "vfs_file_1", content = "VFS sdcard module works normally !!!") succeeded

2. Read vfs file succeeded

3. Delete vfs file succeeded

# Note

None

# Supported IC

RTL8730E
RTL8721F
RTL8735C
