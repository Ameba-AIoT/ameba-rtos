# Example Description

This is an example of using virtual file system on SPI-compatible SD NAND Flash.

# HW Configuration

None

# SW configuration

1. Fixed Parameters for SPI-compatible SD NAND Flash Registration:
   When registering FATFS on SPI-compatible SD NAND Flash using:
   `vfs_user_register("sd_spi", VFS_FATFS, VFS_INF_SD_SPI, VFS_REGION_4, VFS_RW);`
   the arguments `VFS_FATFS`, `VFS_INF_SD_SPI` and `VFS_REGION_4` must remain unchanged.

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

1. Write vfs file (file_name = "vfs_file_1", content = "VFS sd_spi module works normally !!!") succeeded

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
