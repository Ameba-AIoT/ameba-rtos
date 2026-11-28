# Example Description

This is an example of using virtual file system on SD card.

# HW Configuration

Insert an SD card into the SD card slot of the board.

# SW configuration

1. Fixed Parameters for SD Card Registration:
   The SD card is registered with:
   `vfs_target_register("sdcard", VFS_TARGET_SD, VFS_RW);`
   the argument `VFS_TARGET_SD` must remain unchanged. The file system on the card is selected in menuconfig (`CONFIG_VFS_SD_FATFS`).

2. `prj.conf` (applied automatically when building this example) enables the SD card, FATFS, SD card hotplug and exFAT:
   ```
   CONFIG_VFS_SD_CARD=y
   CONFIG_VFS_SD_FATFS=y
   CONFIG_VFS_SD_CARD_HOTPLUG=y
   CONFIG_FATFS_SUPPORT_EXFAT=y
   ```

3. Build and Download:
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
