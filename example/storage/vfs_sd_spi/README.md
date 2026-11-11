# Example Description

This is an example of using virtual file system on SPI-compatible SD NAND Flash.

# HW Configuration

None

# SW configuration

1. Fixed Parameters for SPI-compatible SD NAND Flash Registration:
   When registering FATFS on SPI-compatible SD NAND Flash using:
   `vfs_target_register("sd_spi", VFS_TARGET_SD_SPI, VFS_RW);`
   the argument `VFS_TARGET_SD_SPI` must remain unchanged. The file system (FATFS) is decided by Kconfig (`CONFIG_VFS_SD_SPI`), not by the caller.

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
RTL8720F
