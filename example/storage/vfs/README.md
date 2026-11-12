# Example Description

This example writes, reads back and deletes a file on the on-chip flash VFS targets registered at boot:

- `VFS_TARGET_FLASH_VFS1` (prefix `vfs`, always present)
- `VFS_TARGET_FLASH_VFS2` (prefix `vfs2`, only when VFS2 is enabled in menuconfig)

External media have their own examples: `vfs_sdcard`, `vfs_sd_spi`, `vfs_second_flash`, `vfs_usbh`, `vfs_ram`.

# HW Configuration

None

# SW configuration

1. Default build (`./ameba.py build -a vfs`) tests VFS1 only (LittleFS).

2. To also test VFS2, select its file system in menuconfig under `CONFIG File System` -> `VFS2 partition`, or build with the bundled config:
   ```
   ./ameba.py menuconfig -f example/storage/vfs/fatfs.conf
   ./ameba.py build -a vfs
   ```
   `fatfs.conf` sets `CONFIG_VFS2_FATFS=y`. The flash layout must reserve a VFS2 partition, otherwise registration fails with `no target partition`.

3. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

For each mounted target (`test on vfs`, then `test on vfs2` if enabled):

1. Write vfs file (file_name = "vfs_file_1", content = "VFS module works normally !!!") succeeded

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
RTL8735C
RTL8720F
