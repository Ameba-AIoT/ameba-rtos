# Example Description

This is an example of using virtual file system on the second flash (including data flash).

# HW Configuration

None

# SW configuration
1. Fixed Parameters for Second Flash Registration:
   When registering FATFS on the secondary flash using:
   `vfs_user_register("Second", VFS_FATFS, VFS_INF_SECOND_FLASH, VFS_REGION_4, VFS_RW);`
   the arguments `VFS_INF_SECOND_FLASH` and `VFS_REGION_4` must remain unchanged.

2. Filesystem-Specific Configuration Dependencies:
   If selecting `VFS_FATFS` or `VFS_LITTLEFS` as the filesystem type, corresponding adjustments must be made to the settings in `prj.conf` to ensure compatibility.

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
