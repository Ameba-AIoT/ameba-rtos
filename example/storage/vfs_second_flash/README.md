# Example Description

This is an example of using virtual file system on the second flash (including data flash).

# HW Configuration

None

# SW configuration
1. Fixed Parameters for Second Flash Registration:
   When registering FATFS on the secondary flash using:
   `vfs_target_register("Second", VFS_TARGET_SECOND_FLASH, VFS_RW);`
   the argument `VFS_TARGET_SECOND_FLASH` must remain unchanged.

2. Filesystem-Specific Configuration Dependencies:
   The file system on the second flash is selected in `prj.conf` (`CONFIG_VFS_SECOND_FLASH_FATFS` or `CONFIG_VFS_SECOND_FLASH_LITTLEFS`), not by the caller.

3. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

1. Write vfs file (file_name = "vfs_file_1", content = "VFS second flash module works normally !!!") succeeded

2. Read vfs file succeeded

3. Delete vfs file succeeded

# Note

None

# Supported IC

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
RTL8721F
RTL8720F
